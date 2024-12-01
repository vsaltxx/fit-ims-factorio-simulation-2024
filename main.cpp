#include "simlib.h"
#include <iostream>

// Constants
const double SIMULATION_TIME = 12; // Total simulation time in seconds

const double MINING_TIME = 2.0;   // Time to mine iron ore
const double SMELTING_TIME = 0.6; // Time to smelt iron ore into an iron plate
const double WATER_PROCESS_TIME = 1.0;      // Time to process 200 units of water
const double PUMPJACK_PROCESS_TIME = 1.0;  // Time for the pumpjack to process oil (1 second)
const double OIL_REFINING_TIME = 5.0;       // Time to refine crude oil into petroleum gas
const double CH_PLANT_TIME = 1.0;

const int WATER_UNIT_BATCH = 1200;     // Number of units of water processed per time unit
const int CRUDE_OIL_BATCH = 100;          // Amount of crude oil produced per batch
const int PETROLEUM_GAS_BATCH = 45;         // Amount of petroleum gas produced per refining process
const int SULFUR_BATCH = 2;

const int  MIN_CRUDE_OIL_FOR_GAS = 100;
const int  MIN_WATER_FOR_SULFUR = 30;
const int  MIN_GAS_FOR_SULFUR = 30;

const int NUM_FURNACES = 1;       // Number of furnaces
const int NUM_DRILLS = 4;         // Number of mining drills
const int NUM_WATER_PUMPS = 1;
const int NUM_PUMPJACKS = 1;
const int NUM_OIL_REFINERIES = 1;           // Number of oil refineries
const int NUM_CH_PLANTS = 1;


// Global variables
int iron_ore_mined = 0;           // Counter for mined iron ore
int iron_ore_left = 0;           // Counter for mined iron ore left at the end of the simulation

int iron_plates_produced = 0;      // Counter for smelted iron plates
int water_units_produced = 0;    // Counter for processed water units
int water_units_left = 0;

int crude_oil_produced = 0;               // Counter for total crude oil produced
int crude_oil_left = 0;               // Counter for crude oil left at the end of the simulation

int petroleum_gas_produced = 0;             // Counter for total petroleum gas produced
int petroleum_gas_left = 0;

int sulfur_produced = 0;

// Queue
Queue ironOreQueue("Iron Ore Queue");
Queue ironPlateQueue("Iron Plate Queue");
Queue waterQueue("Water Queue");
Queue crudeOilQueue("Crude Oil Queue");
Queue sulfurQueue("Sulfur Queue");
Queue petroleumGasQueue("Petroleum Gas Queue");


// Store
Store miningDrills("Electric Mining Drills", NUM_DRILLS); // Store for mining drills
Store electricalFurnaces("Electrical Furnaces", NUM_FURNACES); // Store for furnaces
Store waterWellPump("Water Well Pump", NUM_WATER_PUMPS);    // Store for water well pumps
Store pumpjack("Pumpjack", NUM_PUMPJACKS);    // Store for pumpjacks
Store oilRefinery("Oil Refinery", NUM_OIL_REFINERIES); // Store for oil refineries
Store chemicalPlant("Chemical Plant", NUM_CH_PLANTS);


class SulfurProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions itself
    }
};

class SulfurProductionProcess : public Process {
    void Behavior() override {
        if (waterQueue.Length() >= 30 && petroleumGasQueue.Length() >= 30) {
            for (int i = 0; i < 30; ++i) {
                auto *water = waterQueue.GetFirst(); // Remove 30 units of water from the queue
                delete water;                       // Free the memory

                auto *petroleumGas = petroleumGasQueue.GetFirst(); // Remove 30 units of petroleum gas from the queue
                delete petroleumGas;                          // Free the memory
            }
            water_units_left -= 30;
            petroleum_gas_left -= 30;

            Enter(chemicalPlant);            // Seize the chemical plant
            Wait(CH_PLANT_TIME);             // Chemical processing time (1 second)
            Leave(chemicalPlant);            // Release the chemical plant

            for (int i = 0; i < 2; ++i) {
                auto *sulfur = new SulfurProcess(); // Create 2 units of sulfur
                sulfurQueue.Insert(sulfur);         // Insert the sulfur into the queue
            }
            sulfur_produced += 2; // Increment sulfur production counter

            std::cout << "Sulfur produced and added to the queue. Total sulfur: "
                      << sulfur_produced << " units.\n";

            // Check again if more resources are available for another sulfur process
            if (Time + CH_PLANT_TIME <= SIMULATION_TIME && waterQueue.Length() >= 30 && petroleumGasQueue.Length() >= 30 && chemicalPlant.Free() > 0) {
                (new SulfurProductionProcess())->Activate();
            }
        }
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
        if (crudeOilQueue.Length() >= 100) {
            for (int i = 0; i < 100; ++i) {
                auto *crudeOil = crudeOilQueue.GetFirst(); // Remove 100 units of crude oil from the queue
                delete crudeOil;                          // Free the memory
            }
            crude_oil_left -= 100;

            Enter(oilRefinery);            // Seize an oil refinery
            Wait(OIL_REFINING_TIME);       // Refining takes 5 seconds
            Leave(oilRefinery);            // Release the oil refinery

            for (int i = 0; i < 45; ++i) {
                auto *petroleumGas = new CrudeOilBatchProcess(); // Create a unique petroleum gas process
                petroleumGasQueue.Insert(petroleumGas);          // Insert the petroleum gas into the queue
            }
            petroleum_gas_produced += 45; // Increment petroleum gas production counter
            petroleum_gas_left += 45;

            std::cout << "Petroleum gas produced and added to the queue. Total petroleum gas: "
                      << petroleum_gas_produced << " units.\n";
            if(Time + OIL_REFINING_TIME < SIMULATION_TIME && chemicalPlant.Free() > 0){
                (new SulfurProductionProcess())->Activate();
            }
        }
    }
};

class PumpjackProcess : public Process {
    void Behavior() override {
        Enter(pumpjack);                  // Enter the pumpjack store
        Wait(PUMPJACK_PROCESS_TIME);      // Processing takes 1 second
        Leave(pumpjack);                  // Leave the pumpjack store

        for (int i = 0; i < 100; ++i) {
            auto *oilBatch = new CrudeOilBatchProcess(); // Create a unique crude oil batch process
            crudeOilQueue.Insert(oilBatch);              // Insert the batch into the crude oil queue
        }
        crude_oil_produced += 100;  // Increment the processed crude oil units counter
        crude_oil_left += 100;
        if (Time + PUMPJACK_PROCESS_TIME < SIMULATION_TIME && oilRefinery.Free() > 0) {
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

        for (int i = 0; i < 1200; ++i) {
            auto *waterBatch = new WaterBatchProcess(); // Create a unique water batch process
            waterQueue.Insert(waterBatch);              // Insert the batch into the water queue
        }
        water_units_produced += 1200;  // Increment the processed water units counter
        water_units_left += 1200;

        // Activate sulfur production if conditions are met
        if (Time + CH_PLANT_TIME <= SIMULATION_TIME && chemicalPlant.Free() > 0) {
            (new SulfurProductionProcess())->Activate();
        }

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
    std::cout << "Simulation finished \n";

    std::cout << "Iron Ore mined during the simulation time:: " << iron_ore_mined << "\n";
    std::cout << "Total iron ore left: " << iron_ore_left << " units.\n";

    std::cout << "Iron Plates created: " << iron_plates_produced << "\n";

    std::cout << "Water units processed during the simulation time:: " << water_units_produced << "\n";
    std::cout << "Water unitss left: " << water_units_left << " units.\n";

    std::cout << "Total crude oil produced during the simulation time: " << crude_oil_produced << " units.\n";
    std::cout << "Total crude oil left: " << crude_oil_left << " units.\n";

    std::cout << "Total petroleum gas produced: " << petroleum_gas_produced << " units.\n";
    std::cout << "Total petroleum gas left: " << petroleum_gas_left << " units.\n";

    std::cout << "Total sulfur produces: " << sulfur_produced << " units.\n";

    oilRefinery.Output();
    pumpjack.Output();
   // std::cout << "Processes in crudeOilQueue: " << waterQueue.Length() << "\n";
    //miningDrills.Output();
    //electricalFurnaces.Output();
    //waterWellPump.Output();
    //best_values();
    return 0;
}