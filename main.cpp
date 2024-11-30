#include "simlib.h"
#include <iostream>

// Constants
const double MINING_TIME = 2.0;   // Time to mine iron ore
const double SMELTING_TIME = 0.6;   // Time to smelt iron ore into an iron plate
const double SIMULATION_TIME = 100; // Total simulation time in seconds


// Global variables
int iron_ore_mined = 0;           // Counter for mined iron ore
int iron_plates_created = 0;         // Counter for smelted iron plates

// Queue
Queue ironOreQueue("Iron Ore Queue");
Queue ironPlateQueue("Iron Plate Queue");

// Facility
Facility miningDrill("Electric Mining Drill");
Facility electricalFurnace("Electrical Furnace");

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

            Seize(electricalFurnace);            // Seize the electrical furnace
            Wait(SMELTING_TIME);                 // Smelting takes 0.6 seconds
            Release(electricalFurnace);          // Release the furnace

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
        Seize(miningDrill);          // Seize the mining drill
        Wait(MINING_TIME);           // Mining takes 2 seconds
        Release(miningDrill);        // Release the mining drill

        auto *ore = new OreProcess(); // Create a unique ore process
        ironOreQueue.Insert(ore);   // Insert mined iron ore into the queue
        iron_ore_mined++;            // Increment mined iron ore counter

        std::cout << "Iron ore mined and added to the queue. Total mined: " << iron_ore_mined << "\n";
        std::cout << "Processes in ironOreQueue: " << ironOreQueue.Length() << "\n";

        // If furnace is not busy, activate smelting process
        if (!electricalFurnace.Busy() && !ironOreQueue.Empty()) {
            (new SmeltingProcess())->Activate();
        }

    }
};

// Event class: Resource generation
class ResourceGenerator : public Event {
    void Behavior() override {
        // Activate a new mining process
        (new MiningProcess())->Activate();

        // Continue generating resources until the simulation time ends
        Activate(Time + MINING_TIME); // Schedule the next mining event
    }
};

// Main function
int main() {
    // Initialize simulation
    Init(0,SIMULATION_TIME); // Simulation time from 0 to 1000 seconds

    // Start the resource generation process
    (new ResourceGenerator())->Activate();

    // Run the simulation
    Run();

    // Print results
    std::cout << "Simulation finished. Iron Ore mined: " << iron_ore_mined << "\n";
    std::cout << "Simulation finished. Iron Plates created: " << iron_plates_created << "\n";
    miningDrill.Output();
    electricalFurnace.Output();

    return 0;
}
