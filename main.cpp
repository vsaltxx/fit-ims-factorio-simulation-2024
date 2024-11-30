#include "simlib.h"
#include <iostream>

// Constants
const double MINING_TIME = 2.0;   // Time to mine iron ore
const double SMELTING_TIME = 0.6; // Time to smelt iron ore into an iron plate
const double SIMULATION_TIME = 10; // Total simulation time in seconds
const double WATER_PROCESS_TIME = 1.0;      // Time to process 200 units of water
const int WATER_UNIT_BATCH = 200;     // Number of units of water processed per time unit


const int NUM_FURNACES = 1;       // Number of furnaces
const int NUM_DRILLS = 4;         // Number of mining drills
const int NUM_WATER_PUMPS = 1;

// Global variables
int iron_ore_mined = 0;           // Counter for mined iron ore
int iron_plates_created = 0;      // Counter for smelted iron plates
int water_units_processed = 0;    // Counter for processed water units

// Queue
Queue ironOreQueue("Iron Ore Queue");
Queue ironPlateQueue("Iron Plate Queue");
Queue waterQueue("Water Queue");

// Store
Store miningDrills("Electric Mining Drills", NUM_DRILLS); // Store for mining drills
Store electricalFurnaces("Electrical Furnaces", NUM_FURNACES); // Store for furnaces
Store waterWellPump("Water Well Pump", NUM_WATER_PUMPS);    // Store for water well pumps



// Dummy process class for processed water
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

        std::cout << "Water batch processed and added to the queue. Total water units processed: "
                  << water_units_processed << "\n";

        // Activate the next water production process
        if (Time < SIMULATION_TIME) {
            (new WaterProductionProcess())->Activate();
        }
    }
};

// Dummy process class for smelted iron plates
class PlateProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions
    }
};

class SmeltingProcess : public Process {
    void Behavior() override {
        if (!ironOreQueue.Empty()) {
            auto *ore = ironOreQueue.GetFirst(); // Take iron ore from the queue
            delete ore;                          // Remove the ore from memory

            Enter(electricalFurnaces);           // Enter the store (seize a furnace)
            Wait(SMELTING_TIME);                 // Smelting takes 0.6 seconds
            Leave(electricalFurnaces);           // Leave the store (release the furnace)

            auto *plate = new PlateProcess();    // Create a unique plate process
            ironPlateQueue.Insert(plate);        // Insert the iron plate into the queue
            iron_plates_created++;               // Increment the iron plates counter

            std::cout << "Iron plate created and added to the queue. Total plates: " << iron_plates_created << "\n";
            std::cout << "Processes in ironPlateQueue: " << ironPlateQueue.Length() << "\n";

            // If more iron ore is available, activate another smelting process
            if (!ironOreQueue.Empty()) {
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

        std::cout << "Iron ore mined and added to the queue. Total mined: " << iron_ore_mined << "\n";
        std::cout << "Processes in ironOreQueue: " << ironOreQueue.Length() << "\n";

        // If any furnace is available, activate smelting process
        if (electricalFurnaces.Free() > 0 && !ironOreQueue.Empty()) {
            (new SmeltingProcess())->Activate();
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

// Main function
int main() {
    // Initialize simulation
    Init(0, SIMULATION_TIME); // Simulation time from 0 to 1000 seconds

    // Start the resource generation process
    (new ResourceGenerator())->Activate();
    (new WaterProductionGenerator())->Activate(); // Activate water production

    // Run the simulation
    Run();

    // Print results
    std::cout << "Simulation finished. Iron Ore mined: " << iron_ore_mined << "\n";
    std::cout << "Simulation finished. Iron Plates created: " << iron_plates_created << "\n";
    std::cout << "Simulation finished. Water units processed: " << water_units_processed << "\n";

    miningDrills.Output();
    electricalFurnaces.Output();
    waterWellPump.Output();

    return 0;
}