#include "simlib.h"
#include <iostream>

// Constants
const double MINING_TIME = 2.0;   // Time to mine iron ore
const int TOTAL_IRON_ORE = 10;    // Total iron ore units to produce

// Global variables
int iron_ore_mined = 0;           // Counter for mined iron ore

// Queue
Queue ironOreQueue("Iron Ore Queue");

// Facility
Facility miningDrill("Electric Mining Drill");

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


        // Check if the queue has any processes
        if (!ironOreQueue.Empty()) {
            std::cout << "Processes in ironOreQueue: " << ironOreQueue.Length() << "\n";
        } else {
            std::cout << "ironOreQueue is empty.\n";
        }
        // Stop simulation when target is achieved
        if (iron_ore_mined >= TOTAL_IRON_ORE) {
            Stop();
        }
    }
};

// Event class: Resource generation
class ResourceGenerator : public Event {
    void Behavior() override {
        // Activate a new mining process
        (new MiningProcess())->Activate();

        // Continue generating resources until the goal is reached
        if (iron_ore_mined < TOTAL_IRON_ORE) {
            Activate(Time + MINING_TIME); // Schedule the next mining event
        }
    }
};

// Main function
int main() {
    // Initialize simulation
    Init(0, 1000); // Simulation time from 0 to 1000 seconds

    // Start the resource generation process
    (new ResourceGenerator())->Activate();

    // Run the simulation
    Run();

    // Print results
    std::cout << "Simulation finished. Iron Ore mined: " << iron_ore_mined << "\n";
    miningDrill.Output();

    return 0;
}
