#include "simlib.h"
#include <iostream>

// Constants
const double SIMULATION_TIME = 10; // Total simulation time in seconds

const double MINING_TIME = 2.0;   // Time to mine iron ore
const double SMELTING_TIME = 0.6; // Time to smelt iron ore into an iron plate
const double WATER_PROCESS_TIME = 1.0;      // Time to process 200 units of water
const double PUMPJACK_PROCESS_TIME = 1.0;  // Time for the pumpjack to process oil (1 second)
const double OIL_REFINING_TIME = 5.0;       // Time to refine crude oil into petroleum gas
const double CH_PLANT_TIME = 1.0;
const double IRON_MINING_TIME = 2.0; 
const double IRON_SMELTING_TIME = 0.6; // Time to smelt iron ore into an iron plate
const double COPPER_MINING_TIME = 2.0;   // Time to mine copper ore
const double COPPER_SMELTING_TIME = 0.6;
const double CABLE_PRODUCTION_TIME = 1.0; 
const double CIRCUIT_ASSEMBLY_TIME = 1.0; 


const int WATER_UNIT_BATCH = 1200;     // Number of units of water processed per time unit
const int CRUDE_OIL_BATCH = 100;          // Amount of crude oil produced per batch
const int PETROLEUM_GAS_BATCH = 45;         // Amount of petroleum gas produced per refining process
const int SULFUR_BATCH = 2;
const int PLASTIC_BAR_BATCH = 2;


const int  MIN_CRUDE_OIL_FOR_GAS = 100;
const int  MIN_WATER_FOR_SULFUR = 30;
const int  MIN_GAS_FOR_SULFUR = 30;
const int  MIN_GAS_FOR_PLASTIC_BAR = 20;
const int  MIN_COAL_FOR_PLASTIC_BAR = 1;


const int NUM_FURNACES = 1;
const int NUM_IRON_ORE_DRILLS = 4; // Number of iron ore mining drills// Number of furnaces
const int NUM_COAL_DRILLS = 1;     // Number of coal mining drills         // Number of mining drills
const int NUM_WATER_PUMPS = 1;
const int NUM_PUMPJACKS = 1;
const int NUM_PUMPJACKS_FOR_PLASTIC_BAR = 1; // Number of new pumpjacks
const int NUM_OIL_REFINERIES = 1;           // Number of oil refineries
const int NUM_SULFUR_PLANTS = 1; // Number of sulfur chemical plants
const int NUM_PLASTIC_PLANTS = 1; // Number of plastic chemical plants
const int NUM_COPPER_FURNACES = 1;       // Number of furnaces for copper
const int NUM_COPPER_DRILLS = 4; 


const int IRON_NUM_FURNACES = 1;   
const int IRON_NUM_DRILLS = 4; 
const int CABLE_NUM_ASSEMBLY_MACHINES = 1;      // Number of assembly machines
const int ELECTRONIC_CIRCUIT_NUM_ASSEMBLY_MACHINES = 1;    

// Global variables
int iron_ore_produced = 0;           // Counter for mined iron ore
int iron_plates_produced = 0;      // Counter for smelted iron plates
int water_units_produced = 0;    // Counter for processed water units
int crude_oil_produced = 0;               // Counter for total crude oil produced
int petroleum_gas_produced = 0;             // Counter for total petroleum gas produced
int sulfur_produced = 0;
int coal_produced = 0;
int crude_oil_for_plastic_bar_produced = 0;
int petroleum_gas_for_plastic_bar_produced = 0;
int plastic_bar_produced = 0;

int iron_ore_mined = 0;           // Counter for mined iron ore
int iron_plates_created = 0;      // Counter for smelted iron plates
int copper_ore_mined = 0;         // Counter for mined copper ore
int copper_plates_created = 0;    
int copper_cables_created = 0;    
int electronic_circuits_created = 0; // Counter for created electronic circuits

bool CopperToggleSplitter = true; 
bool IronToggleSplitter = true;   


// Queues
Queue ironOreQueue("Iron Ore Queue");
Queue ironPlateQueue("Iron Plate Queue");
Queue waterQueue("Water Queue");
Queue crudeOilQueue("Crude Oil Queue");
Queue sulfurQueue("Sulfur Queue");
Queue petroleumGasQueue("Petroleum Gas Queue");
Queue coalQueue("Coal Queue");

Queue CrudeOilForPlasticBarQueue("New Crude Oil Queue");
Queue PetroleumGasForPlasticBarQueue("New Petroleum Gas Queue");
Queue plasticBarQueue("New Plastic Queue");

Queue copperOreQueue("Copper Ore Queue");
Queue copperPlateQueue("Copper Plate Queue");
Queue copperCableQueue("Copper Cable Queue");
Queue copperCableForElectronicCircuit("Copper Cable Queue 1");
Queue copperCableForAdvanceElectronicCircuit("Copper Cable Queue 2");
Queue ironPlateQueueForCircuit("Iron Plate Queue 2");
Queue ironPlateQueueForSulfur("Iron Plate Queue 1");
Queue electronicCircuitQueue("Electronic Circuit Queue");

// Store
Store ironOreDrills("Iron Ore Mining Drills", NUM_IRON_ORE_DRILLS);
Store electricalFurnaces("Electrical Furnaces", NUM_FURNACES); // Store for furnaces
Store waterWellPump("Water Well Pump", NUM_WATER_PUMPS);    // Store for water well pumps
Store pumpjack("Pumpjack", NUM_PUMPJACKS);    // Store for pumpjacks
Store oilRefinery("Oil Refinery", NUM_OIL_REFINERIES); // Store for oil refineries
Store coalDrills("Coal Mining Drills", NUM_COAL_DRILLS);

Store pumpjackForPlasticBar("Pumpjack for Plastic Bar Production", NUM_PUMPJACKS_FOR_PLASTIC_BAR);
Store oilRefineryForPlasticBar("New Oil Refinery", NUM_OIL_REFINERIES);
Store sulfurChemicalPlant("Sulfur Chemical Plant", NUM_SULFUR_PLANTS);
Store plasticChemicalPlant("Plastic Chemical Plant", NUM_PLASTIC_PLANTS);

Store miningDrills("Iron Electric Mining Drills", IRON_NUM_DRILLS); // Store for mining drills
Store ironElectricalFurnaces("Iron Electrical Furnaces", IRON_NUM_FURNACES); // Store for furnaces
Store copperMiningDrills("Copper Electrical Mining Drills", NUM_COPPER_DRILLS); // Store for copper mining drills
Store copperFurnaces("Copper Electrical Furnaces", NUM_COPPER_FURNACES); // Store for copper furnaces
Store copperCableAssemblyMachines("Assembly Machines", CABLE_NUM_ASSEMBLY_MACHINES); // Store for assembly machines
Store electronicCircuitAssemblyMachines("Assembly Machines", ELECTRONIC_CIRCUIT_NUM_ASSEMBLY_MACHINES); // Store for assembly machines


class SulfurProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions itself
    }
};

class SulfurProductionProcess : public Process {
    void Behavior() override {
        if (waterQueue.Length() >= MIN_WATER_FOR_SULFUR && petroleumGasQueue.Length() >= MIN_GAS_FOR_SULFUR) {
            for (int i = 0; i < MIN_GAS_FOR_SULFUR; ++i) {
                auto *water = waterQueue.GetFirst();
                delete water;

                auto *petroleumGas = petroleumGasQueue.GetFirst();
                delete petroleumGas;
            }

            Enter(sulfurChemicalPlant);
            Wait(CH_PLANT_TIME);
            Leave(sulfurChemicalPlant);

            for (int i = 0; i < SULFUR_BATCH; ++i) {
                auto *sulfur = new SulfurProcess();
                sulfurQueue.Insert(sulfur);
            }
            sulfur_produced += SULFUR_BATCH;

            if (Time + CH_PLANT_TIME <= SIMULATION_TIME && waterQueue.Length() >= MIN_WATER_FOR_SULFUR && petroleumGasQueue.Length() >= MIN_GAS_FOR_SULFUR && sulfurChemicalPlant.Free() > 0) {
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

class OilRefiningProcess : public Process {
    void Behavior() override {
        if (crudeOilQueue.Length() >= MIN_CRUDE_OIL_FOR_GAS) {
            for (int i = 0; i < MIN_CRUDE_OIL_FOR_GAS; ++i) {
                auto *crudeOil = crudeOilQueue.GetFirst();
                delete crudeOil;
            }

            Enter(oilRefinery);
            Wait(OIL_REFINING_TIME);
            Leave(oilRefinery);

            for (int i = 0; i < PETROLEUM_GAS_BATCH; ++i) {
                auto *petroleumGas = new CrudeOilBatchProcess();
                petroleumGasQueue.Insert(petroleumGas);
            }
            petroleum_gas_produced += PETROLEUM_GAS_BATCH;

            if(Time + OIL_REFINING_TIME < SIMULATION_TIME && sulfurChemicalPlant.Free() > 0){
                (new SulfurProductionProcess())->Activate();

            }
        }
    }
};

class PumpjackProcess : public Process {
    void Behavior() override {
        Enter(pumpjack);
        Wait(PUMPJACK_PROCESS_TIME);
        Leave(pumpjack);

        for (int i = 0; i < MIN_CRUDE_OIL_FOR_GAS; ++i) {
            auto *oilBatch = new CrudeOilBatchProcess();
            crudeOilQueue.Insert(oilBatch);
        }
        crude_oil_produced += CRUDE_OIL_BATCH;
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

        for (int i = 0; i < WATER_UNIT_BATCH; ++i) {
            auto *waterBatch = new WaterBatchProcess(); // Create a unique water batch process
            waterQueue.Insert(waterBatch);              // Insert the batch into the water queue
        }
        water_units_produced += WATER_UNIT_BATCH;  // Increment the processed water units counter

        // Activate sulfur production if conditions are met
        if (Time + CH_PLANT_TIME <= SIMULATION_TIME && sulfurChemicalPlant.Free() > 0) {
            (new SulfurProductionProcess())->Activate();
        }

        // Activate the next water production process
        if (Time + WATER_PROCESS_TIME < SIMULATION_TIME) {
            (new WaterProductionProcess())->Activate();
        }
    }
};

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

            Enter(ironElectricalFurnaces);           // Enter the store (seize a furnace)
            Wait(IRON_SMELTING_TIME);            // Smelting takes 0.6 seconds
            Leave(ironElectricalFurnaces);           // Leave the store (release the furnace)

            auto *plate = new PlateProcess();    // Create a unique plate process
            ironPlateQueue.Insert(plate);        // Insert the iron plate into the queue
            iron_plates_produced++;               // Increment the iron plates counter

            // std::cout << "Iron plate created and added to the queue. Total plates: " << iron_plates_produced << "\n";
            // std::cout << "Processes in ironPlateQueue: " << ironPlateQueue.Length() << "\n";
         
            if (!ironPlateQueue.Empty()) {
                (new IronPlateSplitter())->Activate(); // Activate the iron plate splitter process
            }

            if (Time + IRON_SMELTING_TIME <= SIMULATION_TIME && !ironOreQueue.Empty()) {
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

            if (Time + CABLE_PRODUCTION_TIME <= SIMULATION_TIME && !copperPlateQueue.Empty()) {
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

            if (Time + COPPER_SMELTING_TIME <= SIMULATION_TIME && !copperOreQueue.Empty()) {
                (new CopperSmeltingProcess())->Activate(); // Activate another smelting process if more copper ore is available
            }

            if (Time + CABLE_PRODUCTION_TIME <= SIMULATION_TIME && copperCableAssemblyMachines.Free() > 0 && !copperPlateQueue.Empty()) {
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

        if (Time + IRON_SMELTING_TIME <= SIMULATION_TIME && ironElectricalFurnaces.Free() > 0 && !ironOreQueue.Empty()) {
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

        if (Time + COPPER_SMELTING_TIME <= SIMULATION_TIME && copperFurnaces.Free() > 0 && !copperOreQueue.Empty()) {
            (new CopperSmeltingProcess())->Activate(); // Activate smelting process if any furnace is available
        }
    }
};

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


class PlasticProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions itself
    }
};

class PlasticProductionProcess : public Process {
    void Behavior() override {
        if (PetroleumGasForPlasticBarQueue.Length() >= MIN_GAS_FOR_PLASTIC_BAR && coalQueue.Length() >= MIN_COAL_FOR_PLASTIC_BAR) {
            for (int i = 0; i < MIN_GAS_FOR_PLASTIC_BAR; ++i) {
                auto *petroleumGas = PetroleumGasForPlasticBarQueue.GetFirst();
                delete petroleumGas;
            }
            for (int i = 0; i < MIN_COAL_FOR_PLASTIC_BAR; ++i) {
                auto *coal = coalQueue.GetFirst();
                delete coal;
            }

            Enter(plasticChemicalPlant);
            Wait(CH_PLANT_TIME);
            Leave(plasticChemicalPlant);

            for (int i = 0; i < PLASTIC_BAR_BATCH; ++i) {
                auto *plastic = new PlasticProcess();
                plasticBarQueue.Insert(plastic);
            }
            plastic_bar_produced += PLASTIC_BAR_BATCH;

            if (Time + CH_PLANT_TIME <= SIMULATION_TIME && PetroleumGasForPlasticBarQueue.Length() >= 20 && coalQueue.Length() >= 1 && plasticChemicalPlant.Free() > 0) {
                (new PlasticProductionProcess())->Activate();
            }
        }
    }
};

class CrudeOilForPlasticBarBatchProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions itself
    }
};

class OilRefiningForPlasticBarProcess : public Process {
    void Behavior() override {
        if (CrudeOilForPlasticBarQueue.Length() >= MIN_CRUDE_OIL_FOR_GAS) {
            for (int i = 0; i < MIN_CRUDE_OIL_FOR_GAS; ++i) {
                auto *crudeOil = CrudeOilForPlasticBarQueue.GetFirst();
                delete crudeOil;
            }

            Enter(oilRefineryForPlasticBar);
            Wait(OIL_REFINING_TIME);
            Leave(oilRefineryForPlasticBar);

            for (int i = 0; i < PETROLEUM_GAS_BATCH; ++i) {
                auto *petroleumGas = new CrudeOilForPlasticBarBatchProcess();
                PetroleumGasForPlasticBarQueue.Insert(petroleumGas);
            }
            petroleum_gas_for_plastic_bar_produced += PETROLEUM_GAS_BATCH;

            if (plasticChemicalPlant.Free() > 0) {
                (new PlasticProductionProcess())->Activate();
            }
        }
    }
};


class PumpjackProcessForPlasticBar : public Process {
    void Behavior() override {
        Enter(pumpjackForPlasticBar);
        Wait(PUMPJACK_PROCESS_TIME);
        Leave(pumpjackForPlasticBar);

        for (int i = 0; i < CRUDE_OIL_BATCH; ++i) {
            auto *oilBatch = new CrudeOilForPlasticBarBatchProcess();
            CrudeOilForPlasticBarQueue.Insert(oilBatch);
        }
        crude_oil_for_plastic_bar_produced += CRUDE_OIL_BATCH;
        if (Time + OIL_REFINING_TIME < SIMULATION_TIME && oilRefineryForPlasticBar.Free() > 0) {
            (new OilRefiningForPlasticBarProcess())->Activate();
        }
    }
};

class OilProductionForPlasticBarGenerator : public Event {
    void Behavior() override {
        (new PumpjackProcessForPlasticBar())->Activate();

        if (Time + PUMPJACK_PROCESS_TIME < SIMULATION_TIME) {
            Activate(Time + PUMPJACK_PROCESS_TIME);
        }
    }
};

class CoalProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions itself
    }
};

class CoalMiningProcess : public Process {
    void Behavior() override {
        Enter(coalDrills);
        Wait(MINING_TIME);
        Leave(coalDrills);

        auto *coal = new CoalProcess();
        coalQueue.Insert(coal);
        coal_produced++;

        if (Time + MINING_TIME <= SIMULATION_TIME && coalDrills.Free() > 0) {
            (new CoalMiningProcess())->Activate();
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
        for (int i = 0; i < NUM_COAL_DRILLS; ++i) {
            (new CoalMiningProcess())->Activate();
        }
        if (Time + IRON_MINING_TIME < SIMULATION_TIME) {
            Activate(Time + IRON_MINING_TIME); // Schedule the next mining event
        }
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
    // Initialize simulation
    Init(0, SIMULATION_TIME);

    // Start the resource generation process
    (new OilProductionGenerator())->Activate();
    (new WaterProductionGenerator())->Activate();
    (new ResourceGenerator())->Activate();
    (new OilProductionForPlasticBarGenerator())->Activate();

    Run(); // Run the simulation

    // Print results
    std::cout << "Simulation finished \n";

//    std::cout << "Iron Ore mined during the simulation time: " << iron_ore_mined << "\n";
//    std::cout << "Total iron ore left: " << iron_ore_left << " units.\n";
//
//    std::cout << "Iron Plates created: " << iron_plates_produced << "\n";
//
//    std::cout << "Water units processed during the simulation time:: " << water_units_produced << "\n";
//    std::cout << "Water unitss left: " << water_units_left << " units.\n";
//
//    std::cout << "Total crude oil produced during the simulation time: " << crude_oil_produced << " units.\n";
//    std::cout << "Total crude oil left: " << crude_oil_left << " units.\n";
//
//    std::cout << "Total petroleum gas produced: " << petroleum_gas_produced << " units.\n";
//    std::cout << "Total petroleum gas left: " << petroleum_gas_left << " units.\n";

//    std::cout << "Total sulfur produced: " << sulfur_produced << " units.\n";
//    std::cout << "Sulfur units in a queue: " << sulfurQueue.Length() << "\n\n";

    std::cout << "Total coal produced: " <<  coal_produced << " units.\n";
    std::cout << "Coal units in a queue (left): " << coalQueue.Length() << "\n\n";
    coalDrills.Output();

    std::cout << "Total crude oil for plastic bar produced: " <<  crude_oil_for_plastic_bar_produced << " units.\n";
    std::cout << "Crude oil units left: " << CrudeOilForPlasticBarQueue.Length() << "\n\n";
    pumpjackForPlasticBar.Output();

    std::cout << "Total petroleum gas for plastic produced: " << petroleum_gas_produced << " units.\n";
    std::cout << "Petroleum gas for plastic left: " << PetroleumGasForPlasticBarQueue.Length() << "\n\n";
    oilRefineryForPlasticBar.Output();

    std::cout << "Total plastic bars produced: " << plastic_bar_produced << " units.\n";
    std::cout << "Plastic bar left: " << plasticBarQueue.Length() << "\n\n";
    plasticChemicalPlant.Output();

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

    //electricalFurnaces.Output();
    //waterWellPump.Output();
    //best_values();

    return 0;
}