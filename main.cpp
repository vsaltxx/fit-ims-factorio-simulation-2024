#include "simlib.h"
#include <iostream>

// Constants
const double SIMULATION_TIME = 10; // Total simulation time in seconds

const double MINING_TIME = 2.0;   // Time to mine iron ore
const double SMELTING_TIME = 0.6; // Time to smelt iron ore into an iron plate
const double WATER_PROCESS_TIME = 1.0;      // Time to process 200 units of water
const double PUMPJACK_PROCESS_TIME = 1.0;  // Time for the pumpjack to process oil (1 second)
const double OIL_REFINING_TIME = 5.0;       // Time to refine crude oil into petroleum gas


const int WATER_UNIT_BATCH = 1200;     // Number of units of water processed per time unit
const int CRUDE_OIL_BATCH = 100;          // Amount of crude oil produced per batch
const int PETROLEUM_GAS_BATCH = 45;         // Amount of petroleum gas produced per refining process

const int  MIN_CRUDE_OIL_FOR_GAS = 100;

const int NUM_FURNACES = 1;       // Number of furnaces
const int NUM_DRILLS = 4;         // Number of mining drills
const int NUM_WATER_PUMPS = 1;
const int NUM_PUMPJACKS = 1;
const int NUM_OIL_REFINERIES = 1;           // Number of oil refineries


// Global variables
int iron_ore_mined = 0;           // Counter for mined iron ore
int iron_ore_left = 0;           // Counter for mined iron ore left at the end of the simulation
int iron_plates_produced = 0;      // Counter for smelted iron plates
int water_units_processed = 0;    // Counter for processed water units
int crude_oil_produced = 0;               // Counter for total crude oil produced
int crude_oil_left = 0;               // Counter for crude oil left at the end of the simulation
int petroleum_gas_produced = 0;             // Counter for total petroleum gas produced


// Queue
Queue ironOreQueue("Iron Ore Queue");
Queue ironPlateQueue("Iron Plate Queue");
Queue waterQueue("Water Queue");
Queue crudeOilQueue("Crude Oil Queue");
Queue petroleumGasQueue("Petroleum Gas Queue");


// Store
Store miningDrills("Electric Mining Drills", NUM_DRILLS); // Store for mining drills
Store electricalFurnaces("Electrical Furnaces", NUM_FURNACES); // Store for furnaces
Store waterWellPump("Water Well Pump", NUM_WATER_PUMPS);    // Store for water well pumps
Store pumpjack("Pumpjack", NUM_PUMPJACKS);    // Store for pumpjacks
Store oilRefinery("Oil Refinery", NUM_OIL_REFINERIES); // Store for oil refineries


class PetroleumGasBatchProcess  : public Process {
    void Behavior() override {
        // This process doesn't perform any actions itself
    }
};

class CrudeOilBatchProcess  : public Process {
    void Behavior() override {
        // This process doesn't perform any actions itself
    }
};

// Process class: Oil refining
class OilRefiningProcess : public Process {
    void Behavior() override {
        crude_oil_left -= MIN_CRUDE_OIL_FOR_GAS;
        auto *crudeOil = crudeOilQueue.GetFirst(); // Remove the process from the queue
        delete crudeOil;                          // Free the memory

        Enter(oilRefinery);            // Seize an oil refinery
        Wait(OIL_REFINING_TIME);       // Refining takes 5 seconds
        Leave(oilRefinery);            // Release the oil refinery

        // Create a petroleum gas batch and insert it into the queue
        auto *gasBatch = new PetroleumGasBatchProcess();
        petroleumGasQueue.Insert(gasBatch); // Insert the batch into the petroleum gas queue
        petroleum_gas_produced += PETROLEUM_GAS_BATCH; // Increment petroleum gas production counter

        //std::cout << "Petroleum gas produced: " << petroleum_gas_produced << " units.\n";

        // Check again if we can start another refining process
        if (Time + OIL_REFINING_TIME <= SIMULATION_TIME && crude_oil_left >= MIN_CRUDE_OIL_FOR_GAS && oilRefinery.Free() > 0) {
            (new OilRefiningProcess())->Activate();
        }
    }
};

class PumpjackProcess  : public Process {
    void Behavior() override {
        Enter(pumpjack);                  // Enter the water pump store
        Wait(PUMPJACK_PROCESS_TIME);              // Processing takes 1 second
        Leave(pumpjack);                  // Leave the water pump store

        auto *oilBatch = new CrudeOilBatchProcess(); // Create a unique water batch process
        crudeOilQueue.Insert(oilBatch);              // Insert the batch into the water queue
        crude_oil_produced += CRUDE_OIL_BATCH;  // Increment the processed water units counter
        crude_oil_left += CRUDE_OIL_BATCH;

//        std::cout << "Crude oil batch produced and added to the queue. Total crude oil: "
//                  << crude_oil_produced << " units.\n";
//        std::cout << "Processes in crudeOilQueue: " << crudeOilQueue.Length() << "\n";

        if (Time + OIL_REFINING_TIME <= SIMULATION_TIME && crude_oil_left >= MIN_CRUDE_OIL_FOR_GAS && oilRefinery.Free() > 0) {
            (new OilRefiningProcess())->Activate();
        }
    }
};

class WaterBatchProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions itself
    }
};

class WaterProductionProcess : public Process {
    void Behavior() override {
        Enter(waterWellPump);                  // Enter the water pump store
        Wait(WATER_PROCESS_TIME);              // Processing takes 1 second
        Leave(waterWellPump);                  // Leave the water pump store

        auto *waterBatch = new WaterBatchProcess(); // Create a unique water batch process
        waterQueue.Insert(waterBatch);              // Insert the batch into the water queue
        water_units_processed += WATER_UNIT_BATCH;  // Increment the processed water units counter

        //std::cout << "Water batch processed and added to the queue. Total water units processed: "
        //          << water_units_processed << "\n";

        // Activate the next water production process
        if (Time + WATER_PROCESS_TIME < SIMULATION_TIME) {
            (new WaterProductionProcess())->Activate();
        }
    }
};

class PlateProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions
    }
};

class SmeltingProcess : public Process {
    void Behavior() override {
        if (!ironOreQueue.Empty()) {
            iron_ore_left--;
            auto *ore = ironOreQueue.GetFirst(); // Take iron ore from the queue
            delete ore;                          // Remove the ore from memory

            Enter(electricalFurnaces);           // Enter the store (seize a furnace)
            Wait(SMELTING_TIME);                 // Smelting takes 0.6 seconds
            Leave(electricalFurnaces);           // Leave the store (release the furnace)

            auto *plate = new PlateProcess();    // Create a unique plate process
            ironPlateQueue.Insert(plate);        // Insert the iron plate into the queue
            iron_plates_produced++;               // Increment the iron plates counter

           // std::cout << "Iron plate created and added to the queue. Total plates: " << iron_plates_produced << "\n";
           // std::cout << "Processes in ironPlateQueue: " << ironPlateQueue.Length() << "\n";

            // If more iron ore is available, activate another smelting process
            if (Time + SMELTING_TIME <= SIMULATION_TIME && !ironOreQueue.Empty()) {
                (new SmeltingProcess())->Activate();
            }
        }
    }
};

// Dummy process class for mined iron ore
class OreProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions
    }
};

// Process class: Mining iron ore
class MiningProcess : public Process {
    void Behavior() override {
        Enter(miningDrills);       // Enter the store (seize a mining drill)
        Wait(MINING_TIME);         // Mining takes 2 seconds
        Leave(miningDrills);       // Leave the store (release the mining drill)

        auto *ore = new OreProcess(); // Create a unique ore process
        ironOreQueue.Insert(ore);   // Insert mined iron ore into the queue
        iron_ore_mined++;            // Increment mined iron ore counter
        iron_ore_left++;

        //std::cout << "Iron ore mined and added to the queue. Total mined: " << iron_ore_mined << "\n";
        //std::cout << "Processes in ironOreQueue: " << ironOreQueue.Length() << "\n";

        // If any furnace is available, activate smelting process
        if (Time + SMELTING_TIME <= SIMULATION_TIME && electricalFurnaces.Free() > 0 && !ironOreQueue.Empty()) {
            (new SmeltingProcess())->Activate();
        }
    }
};


// Event class: Oil production generator
class OilProductionGenerator : public Event {
    void Behavior() override {
        // Activate the first pumpjack process
        (new PumpjackProcess())->Activate();

        // Continue generating oil production if within simulation time
        if (Time + PUMPJACK_PROCESS_TIME < SIMULATION_TIME) {
            Activate(Time + PUMPJACK_PROCESS_TIME); // Schedule the next oil production event
        }
    }
};

// Event class: Water production generator
class WaterProductionGenerator : public Event {
    void Behavior() override {
        // Activate the first water production process
        (new WaterProductionProcess())->Activate();

        // Continue generating water until the simulation time ends
        if (Time + WATER_PROCESS_TIME < SIMULATION_TIME) {
            Activate(Time + WATER_PROCESS_TIME); // Schedule the next water production event
        }
    }
};

// Event class: Resource generation
class ResourceGenerator : public Event {
    void Behavior() override {
        // Activate multiple mining processes to utilize all drills
        for (int i = 0; i < NUM_DRILLS; ++i) {
            (new MiningProcess())->Activate();
        }

        // Continue generating resources until the simulation time ends
        if (Time + MINING_TIME < SIMULATION_TIME) {
            Activate(Time + MINING_TIME); // Schedule the next mining event
        }
    }
};



void best_values(){

    int mine_ore = SIMULATION_TIME / MINING_TIME * NUM_DRILLS;
    int smelt_ore = (SIMULATION_TIME-MINING_TIME) /SMELTING_TIME * NUM_FURNACES;
    int water = SIMULATION_TIME * WATER_UNIT_BATCH * NUM_WATER_PUMPS;
    std::cout << "Best values: \n";
    std::cout << "Mining ore: " << mine_ore << "\n";
    std::cout << "Smelting ore: " << smelt_ore << "\n";
    std::cout << "Water: " << water << "\n";
}
// Main function
int main() {
    // Initialize simulation
    Init(0, SIMULATION_TIME); // Simulation time from 0 to 1000 seconds

    // Start the resource generation process
    (new OilProductionGenerator())->Activate();
    (new WaterProductionGenerator())->Activate();
    (new ResourceGenerator())->Activate();

    // Run the simulation
    Run();

    // Print results
    std::cout << "Simulation finished. Iron Ore mined during the simulation time:: " << iron_ore_mined << "\n";
    std::cout << "Simulation finished. Total iron ore left: " << iron_ore_left << " units.\n";
    std::cout << "Simulation finished. Iron Plates created: " << iron_plates_produced << "\n";
    std::cout << "Simulation finished. Water units processed during the simulation time:: " << water_units_processed << "\n";
    std::cout << "Simulation finished. Total crude oil produced during the simulation time: " << crude_oil_produced << " units.\n";
    std::cout << "Simulation finished. Total crude oil left: " << crude_oil_left << " units.\n";
    std::cout << "Simulation finished. Total petroleum gas produced: " << petroleum_gas_produced << " units.\n";

    oilRefinery.Output();
    pumpjack.Output();
    //miningDrills.Output();
    //electricalFurnaces.Output();
    //waterWellPump.Output();
    //best_values();
    return 0;
}