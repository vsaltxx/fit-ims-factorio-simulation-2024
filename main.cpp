#include "simlib.h"
#include <iostream>

// Iron constants
const double IRON_MINING_TIME = 2.0;   // Time to mine iron ore
const double IRON_SMELTING_TIME = 0.6; // Time to smelt iron ore into an iron plate
const int IRON_NUM_FURNACES = 1;       // Number of furnaces for iron
const int IRON_NUM_DRILLS = 4;         // Number of mining drills for iron

const int SIMULATION_TIME = 10; // Total simulation time in seconds

// Copper constants
const double COPPER_MINING_TIME = 2.0;   // Time to mine copper ore
const double COPPER_SMELTING_TIME = 0.6; // Time to smelt copper ore into a copper plate
const int NUM_COPPER_FURNACES = 1;       // Number of furnaces for copper
const int NUM_COPPER_DRILLS = 4;         // Number of mining drills for copper

// Copper cable constants
const double CABLE_PRODUCTION_TIME = 1.0; // Time to produce copper cables
const int CABLE_NUM_ASSEMBLY_MACHINES = 1;      // Number of assembly machines

// Electronic circuit constants
const double CIRCUIT_ASSEMBLY_TIME = 1.0; // Time to assemble electronic circuits
const int ELECTRONIC_CIRCUIT_NUM_ASSEMBLY_MACHINES = 1;      // Number of assembly machines

// Global variables
int iron_ore_mined = 0;           // Counter for mined iron ore
int iron_plates_created = 0;      // Counter for smelted iron plates
int copper_ore_mined = 0;         // Counter for mined copper ore
int copper_plates_created = 0;    // Counter for smelted copper plates
int copper_cables_created = 0;    // Counter for created copper cables
int electronic_circuits_created = 0; // Counter for created electronic circuits

bool CopperToggleSplitter = true; // Toggle for the splitter
bool IronToggleSplitter = true;   // Toggle for the splitter

// Queues
Queue ironOreQueue("Iron Ore Queue");
Queue ironPlateQueue("Iron Plate Queue");
Queue copperOreQueue("Copper Ore Queue");
Queue copperPlateQueue("Copper Plate Queue");
Queue copperCableQueue("Copper Cable Queue");
Queue copperCableForElectronicCircuit("Copper Cable Queue 1");
Queue copperCableForAdvanceElectronicCircuit("Copper Cable Queue 2");
Queue ironPlateQueueForCircuit("Iron Plate Queue 2");
Queue ironPlateQueueForSulfur("Iron Plate Queue 1");
Queue electronicCircuitQueue("Electronic Circuit Queue");

// Stores
Store miningDrills("Iron Electric Mining Drills", IRON_NUM_DRILLS); // Store for mining drills
Store electricalFurnaces("Iron Electrical Furnaces", IRON_NUM_FURNACES); // Store for furnaces
Store copperMiningDrills("Copper Electrical Mining Drills", NUM_COPPER_DRILLS); // Store for copper mining drills
Store copperFurnaces("Copper Electrical Furnaces", NUM_COPPER_FURNACES); // Store for copper furnaces
Store copperCableAssemblyMachines("Assembly Machines", CABLE_NUM_ASSEMBLY_MACHINES); // Store for assembly machines
Store electronicCircuitAssemblyMachines("Assembly Machines", ELECTRONIC_CIRCUIT_NUM_ASSEMBLY_MACHINES); // Store for assembly machines

// Dummy process classes
class PlateProcess : public Process {
    void Behavior() override {}
};

class CopperPlateProcess : public Process {
    void Behavior() override {}
};

class CopperCableProcess : public Process {
    void Behavior() override {}
};

class ElectronicCircuitProcess : public Process {
    void Behavior() override {}
};

class CircuitAssemblyProcess : public Process {
    void Behavior() override {
        if (!ironPlateQueueForCircuit.Empty() && copperCableForElectronicCircuit.Length() >= 3) {
            auto *plate = ironPlateQueueForCircuit.GetFirst(); // Take iron plate from conveyor 1
            delete plate;                             // Remove the plate from memory

            for (int i = 0; i < 3; ++i) {
                auto *cable = copperCableForElectronicCircuit.GetFirst(); // Take 3 copper cables from conveyor 1
                delete cable;                               // Remove the cable from memory
            }

            Enter(electronicCircuitAssemblyMachines);                   // Enter the store (seize an assembly machine)
            Wait(CIRCUIT_ASSEMBLY_TIME);                                 // Assembly takes 1 second
            Leave(electronicCircuitAssemblyMachines);                   // Leave the store (release the assembly machine)

            auto *circuit = new ElectronicCircuitProcess(); // Create a unique electronic circuit process
            electronicCircuitQueue.Insert(circuit);    // Insert the electronic circuit into the queue
            electronic_circuits_created++;             // Increment the electronic circuits counter

            std::cout << "Electronic circuit created and added to the queue. Total circuits: " << electronic_circuits_created << "\n";
            std::cout << "Processes in electronicCircuitQueue: " << electronicCircuitQueue.Length() << "\n";

            if (!ironPlateQueueForCircuit.Empty() && copperCableForElectronicCircuit.Length() >= 3) {
                (new CircuitAssemblyProcess())->Activate(); // Activate another circuit assembly process if more resources are available
            }
        }
    }
};


class IronPlateSplitter : public Process {
    void Behavior() override {
        while (1) {
            if (!ironPlateQueue.Empty()) {
                auto *plate = ironPlateQueue.GetFirst(); // Take iron plate from the queue

                if (!IronToggleSplitter) {
                    ironPlateQueueForSulfur.Insert(plate); // Insert into conveyor 1
                    std::cout << "Iron plate sent to conveyor 1\n";
                } else {
                    ironPlateQueueForCircuit.Insert(plate); // Insert into conveyor 2
                    std::cout << "Iron plate sent to conveyor 2\n";
                }

                IronToggleSplitter = !IronToggleSplitter; // Toggle the flag for the next plate
            } else {
                Passivate(); // Wait until a plate appears in the queue
            }
        }
    }
};

class IronSmeltingProcess : public Process {
    void Behavior() override {
        if (!ironOreQueue.Empty()) {
            auto *ore = ironOreQueue.GetFirst(); // Take iron ore from the queue
            delete ore;                          // Remove the ore from memory

            Enter(electricalFurnaces);           // Enter the store (seize a furnace)
            Wait(IRON_SMELTING_TIME);            // Smelting takes 0.6 seconds
            Leave(electricalFurnaces);           // Leave the store (release the furnace)

            auto *plate = new PlateProcess();    // Create a unique plate process
            ironPlateQueue.Insert(plate);        // Insert the iron plate into the queue
            iron_plates_created++;               // Increment the iron plates counter

            std::cout << "Iron plate created and added to the queue. Total plates: " << iron_plates_created << "\n";
            std::cout << "Processes in ironPlateQueue: " << ironPlateQueue.Length() << "\n";

            if (!ironPlateQueue.Empty()) {
                (new IronPlateSplitter())->Activate(); // Activate the iron plate splitter process
            }

            if (!ironOreQueue.Empty()) {
                (new IronSmeltingProcess())->Activate(); // Activate another smelting process if more iron ore is available
            }
        }
    }
};

class CopperCableSplitter : public Process {
    void Behavior() override {
        while (1) {
            if (!copperCableQueue.Empty()) {
                auto *cable = copperCableQueue.GetFirst(); // Take copper cable from the queue

                if (!CopperToggleSplitter) {
                    copperCableForAdvanceElectronicCircuit.Insert(cable); // Insert into conveyor 1
                    (new CircuitAssemblyProcess())->Activate(); // Activate the cable process
                    std::cout << "Copper cable sent to conveyor 1\n";
                } else {
                    copperCableForElectronicCircuit.Insert(cable); // Insert into conveyor 2
                    std::cout << "Copper cable sent to conveyor 2\n";
                }

                CopperToggleSplitter = !CopperToggleSplitter; // Toggle the flag for the next cable
            } else {
                Passivate(); // Wait until a cable appears in the queue
            }
        }
    }
};

class CableProductionProcess : public Process {
    void Behavior() override {
        if (!copperPlateQueue.Empty()) {
            auto *plate = copperPlateQueue.GetFirst(); // Take copper plate from the queue
            delete plate;                              // Remove the plate from memory

            Enter(copperCableAssemblyMachines);                   // Enter the store (seize an assembly machine)
            Wait(CABLE_PRODUCTION_TIME);               // Production takes 1 second
            Leave(copperCableAssemblyMachines);                   // Leave the store (release the assembly machine)

            for (int i = 0; i < 2; ++i) {              // Create 2 copper cables from 1 copper plate
                auto *cable = new CopperCableProcess(); // Create a unique cable process
                copperCableQueue.Insert(cable);        // Insert the copper cable into the queue
                copper_cables_created++;               // Increment the copper cables counter
            }

            std::cout << "Copper cables created and added to the queue. Total cables: " << copper_cables_created << "\n";
            std::cout << "Processes in copperCableQueue: " << copperCableQueue.Length() << "\n";

            if (!copperPlateQueue.Empty()) {
                (new CableProductionProcess())->Activate(); // Activate another cable production process if more copper plates are available
            }

            if (!copperCableQueue.Empty()) {
                (new CopperCableSplitter())->Activate(); // Activate the splitter process
            }
        }
    }
};

class CopperSmeltingProcess : public Process {
    void Behavior() override {
        if (!copperOreQueue.Empty()) {
            auto *ore = copperOreQueue.GetFirst(); // Take copper ore from the queue
            delete ore;                            // Remove the ore from memory

            Enter(copperFurnaces);                 // Enter the store (seize a furnace)
            Wait(COPPER_SMELTING_TIME);            // Smelting takes 0.6 seconds
            Leave(copperFurnaces);                 // Leave the store (release the furnace)

            auto *plate = new CopperPlateProcess(); // Create a unique plate process
            copperPlateQueue.Insert(plate);        // Insert the copper plate into the queue
            copper_plates_created++;               // Increment the copper plates counter

            std::cout << "Copper plate created and added to the queue. Total plates: " << copper_plates_created << "\n";
            std::cout << "Processes in copperPlateQueue: " << copperPlateQueue.Length() << "\n";

            if (!copperOreQueue.Empty()) {
                (new CopperSmeltingProcess())->Activate(); // Activate another smelting process if more copper ore is available
            }

            if (copperCableAssemblyMachines.Free() > 0 && !copperPlateQueue.Empty()) {
                (new CableProductionProcess())->Activate(); // Activate cable production process if any assembly machine is available
            }
        }
    }
};


class OreProcess : public Process {
    void Behavior() override {}
};

class CopperOreProcess : public Process {
    void Behavior() override {}
};

class IronMiningProcess : public Process {
    void Behavior() override {
        Enter(miningDrills);       // Enter the store (seize a mining drill)
        Wait(IRON_MINING_TIME);    // Mining takes 2 seconds
        Leave(miningDrills);       // Leave the store (release the mining drill)

        auto *ore = new OreProcess(); // Create a unique ore process
        ironOreQueue.Insert(ore);   // Insert mined iron ore into the queue
        iron_ore_mined++;           // Increment mined iron ore counter

        std::cout << "Iron ore mined and added to the queue. Total mined: " << iron_ore_mined << "\n";
        std::cout << "Processes in ironOreQueue: " << ironOreQueue.Length() << "\n";

        if (electricalFurnaces.Free() > 0 && !ironOreQueue.Empty()) {
            (new IronSmeltingProcess())->Activate(); // Activate smelting process if any furnace is available
        }
    }
};

class CopperMiningProcess : public Process {
    void Behavior() override {
        Enter(copperMiningDrills);  // Enter the store (seize a mining drill)
        Wait(COPPER_MINING_TIME);   // Mining takes 2.5 seconds
        Leave(copperMiningDrills);  // Leave the store (release the mining drill)

        auto *ore = new CopperOreProcess(); // Create a unique ore process
        copperOreQueue.Insert(ore);         // Insert mined copper ore into the queue
        copper_ore_mined++;                 // Increment mined copper ore counter

        std::cout << "Copper ore mined and added to the queue. Total mined: " << copper_ore_mined << "\n";
        std::cout << "Processes in copperOreQueue: " << copperOreQueue.Length() << "\n";

        if (copperFurnaces.Free() > 0 && !copperOreQueue.Empty()) {
            (new CopperSmeltingProcess())->Activate(); // Activate smelting process if any furnace is available
        }
    }
};

class ResourceGenerator : public Event {
    void Behavior() override {
        for (int i = 0; i < IRON_NUM_DRILLS; ++i) {
            (new IronMiningProcess())->Activate(); // Activate multiple mining processes to utilize all drills
        }
        for (int i = 0; i < NUM_COPPER_DRILLS; ++i) {
            (new CopperMiningProcess())->Activate(); // Activate multiple mining processes to utilize all drills
        }

        Activate(Time + IRON_MINING_TIME); // Schedule the next mining event
    }
};

void best_values() {
    int iron_mine_ore = int(SIMULATION_TIME / IRON_MINING_TIME) * IRON_NUM_DRILLS;
    int iron_smelt_ore = int((SIMULATION_TIME - IRON_MINING_TIME) / IRON_SMELTING_TIME) * IRON_NUM_FURNACES;
    int copper_mine_ore = int(SIMULATION_TIME / COPPER_MINING_TIME) * NUM_COPPER_DRILLS;
    int copper_smelt_ore = int((SIMULATION_TIME - COPPER_MINING_TIME) / COPPER_SMELTING_TIME) * NUM_COPPER_FURNACES;
    int copper_cable = int((SIMULATION_TIME - COPPER_SMELTING_TIME - COPPER_MINING_TIME) / CABLE_PRODUCTION_TIME) * 2 * CABLE_NUM_ASSEMBLY_MACHINES;
    int electronic_circuit = int((SIMULATION_TIME - COPPER_SMELTING_TIME - COPPER_MINING_TIME - CABLE_PRODUCTION_TIME) / CIRCUIT_ASSEMBLY_TIME) * ELECTRONIC_CIRCUIT_NUM_ASSEMBLY_MACHINES;
    std::cout << "Best values: \n";
    std::cout << "Mining iron ore: " << iron_mine_ore << "\n";
    std::cout << "Smelting iron ore: " << iron_smelt_ore << "\n";
    std::cout << "Mining copper ore: " << copper_mine_ore << "\n";
    std::cout << "Smelting copper ore: " << copper_smelt_ore << "\n";
    std::cout << "Copper cable production: " << copper_cable << "\n";
    std::cout << "Electronic circuit production: " << electronic_circuit << "\n";
}

int main() {
    Init(0, SIMULATION_TIME); // Initialize simulation

    (new ResourceGenerator())->Activate(); // Start the resource generation process

    Run(); // Run the simulation

    // Print results
    miningDrills.Output();
    electricalFurnaces.Output();
    copperMiningDrills.Output();
    copperFurnaces.Output();
    std::cout << "Simulation finished. Iron Ore mined: " << iron_ore_mined << "\n";
    std::cout << "Simulation finished. Iron Plates created: " << iron_plates_created << "\n";
    std::cout << "Simulation finished. Copper Ore mined: " << copper_ore_mined << "\n";
    std::cout << "Simulation finished. Copper Plates created: " << copper_plates_created << "\n";
    std::cout << "Simulation finished. Copper Cables created: " << copper_cables_created << "\n";
    std::cout << "Copper cables for El.circuit: " << copperCableForElectronicCircuit.Length() << "\n";
    std::cout << "Copper cables for Adv.El.circuit: " << copperCableForAdvanceElectronicCircuit.Length() << "\n";
    std::cout << "Iron plates for Sulfur: " << ironPlateQueueForSulfur.Length() << "\n";
    std::cout << "Iron plates for El.circuit: " << ironPlateQueueForCircuit.Length() << "\n";
    std::cout << "Electronic circuits created: " << electronic_circuits_created << "\n";
    std::cout << "Electronic circuit queue: " << electronicCircuitQueue.Length() << "\n";
    best_values();

    return 0;
}