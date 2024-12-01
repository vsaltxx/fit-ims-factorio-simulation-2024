#include "simlib.h"
#include <iostream>

// Constants
const double SIMULATION_TIME = 10; // Total simulation time in seconds

const double COAL_MINING_TIME = 2.0;
const double WATER_PROCESS_TIME = 1.0;
const double PUMPJACK_PROCESS_TIME = 1.0;
const double OIL_REFINING_TIME = 5.0;
const double CH_PLANT_TIME = 1.0;
const double IRON_MINING_TIME = 2.0; 
const double IRON_SMELTING_TIME = 0.6;
const double COPPER_MINING_TIME = 2.0;
const double COPPER_SMELTING_TIME = 0.6;
const double CABLE_PRODUCTION_TIME = 1.0; 
const double CIRCUIT_ASSEMBLY_TIME = 1.0;

const int WATER_UNIT_BATCH = 1200;
const int CRUDE_OIL_BATCH = 100;
const int PETROLEUM_GAS_BATCH = 45;
const int SULFUR_BATCH = 2;
const int PLASTIC_BAR_BATCH = 2;
const int COPPER_CABLE_BATCH = 2;

const int  MIN_CRUDE_OIL_FOR_GAS = 100;
const int  MIN_WATER_FOR_SULFUR = 30;
const int  MIN_GAS_FOR_SULFUR = 30;
const int  MIN_GAS_FOR_PLASTIC_BAR = 20;
const int  MIN_COAL_FOR_PLASTIC_BAR = 1;
const int  MIN_COPPER_CABLE_FOR_EL_CIRCUIT = 3;

const int NUM_COAL_DRILLS = 4;
const int NUM_WATER_PUMPS = 1;
const int NUM_PUMPJACKS_FOR_SULFUR = 1;
const int NUM_PUMPJACKS_FOR_PLASTIC_BAR = 1;
const int NUM_OIL_REFINERIES_FOR_SULFUR = 1;
const int NUM_OIL_REFINERIES_FOR_PLASTIC_BAR = 1;
const int NUM_SULFUR_PLANTS = 1;
const int NUM_PLASTIC_PLANTS = 1;
const int NUM_COPPER_FURNACES = 1;
const int NUM_COPPER_DRILLS = 4;
const int NUM_IRON_FURNACES = 1;
const int NUM_IRON_ORE_DRILLS = 4;
const int NUM_CABLE_ASSEMBLY_MACHINES = 1;
const int NUM_ELECTRONIC_CIRCUIT_ASSEMBLY_MACHINES = 1;

// Global variables
int iron_plates_produced = 0;
int water_units_produced = 0;
int crude_oil_produced = 0;
int petroleum_gas_produced = 0;
int sulfur_produced = 0;
int coal_produced = 0;
int crude_oil_for_plastic_bar_produced = 0;
int petroleum_gas_for_plastic_bar_produced = 0;
int plastic_bar_produced = 0;
int iron_ore_produced = 0;
int copper_ore_produced = 0;
int copper_plates_produced = 0;
int copper_cables_produced = 0;
int electronic_circuits_produced = 0;

bool CopperToggleSplitter = true; 
bool IronToggleSplitter = true;   


// Queues
Queue ironOreQueue("Iron Ore Queue");
Queue ironPlateQueue("Iron Plate Queue");
Queue waterQueue("Water Queue");
Queue crudeOilForSulfurQueue("Crude Oil  Queue");
Queue sulfurQueue("Sulfur Queue");
Queue petroleumGasForSulfurQueue("Petroleum Gas For Sulfur Queue ");
Queue coalQueue("Coal Queue");
Queue crudeOilForPlasticBarQueue("Crude Oil For Plastic Bar Queue");
Queue petroleumGasForPlasticBarQueue("Petroleum Gas For Plastic Bar Queue");
Queue plasticBarQueue("Plastic Bar Queue");
Queue copperOreQueue("Copper Ore Queue");
Queue copperPlateQueue("Copper Plate Queue");
Queue copperCableQueue("Copper Cable Queue");
Queue copperCableForElectronicCircuit("Copper Cable Queue 1 (For Electronic Circuit)");
Queue copperCableForAdvanceElectronicCircuit("Copper Cable Queue 2 (For Advance Electronic Circuit)");
Queue ironPlateQueueForCircuit("Iron Plate Queue 2 (For Electronic Circuit)");
Queue ironPlateQueueForSulfur("Iron Plate Queue 1 (For Sulfuric Acid)");
Queue electronicCircuitQueue("Electronic Circuit Queue");

// Store
Store waterWellPump("Water Well Pump", NUM_WATER_PUMPS);
Store pumpjackForSulfur("Pumpjack For Sulfur", NUM_PUMPJACKS_FOR_SULFUR);
Store oilRefineryForSulfur("Oil Refinery For Sulfur", NUM_OIL_REFINERIES_FOR_SULFUR);
Store coalDrills("Coal Mining Drills", NUM_COAL_DRILLS);
Store pumpjackForPlasticBar("Pumpjack for Plastic Bar Production", NUM_PUMPJACKS_FOR_PLASTIC_BAR);
Store oilRefineryForPlasticBar("Oil Refinery For Plastic Bar", NUM_OIL_REFINERIES_FOR_PLASTIC_BAR);
Store sulfurChemicalPlant("Sulfur Chemical Plant", NUM_SULFUR_PLANTS);
Store plasticChemicalPlant("Plastic Chemical Plant", NUM_PLASTIC_PLANTS);
Store ironOreDrills("Iron Electric Mining Drills", NUM_IRON_ORE_DRILLS);
Store ironElectricalFurnaces("Iron Electrical Furnaces", NUM_IRON_FURNACES);
Store copperMiningDrills("Copper Electrical Mining Drills", NUM_COPPER_DRILLS);
Store copperFurnaces("Copper Electrical Furnaces", NUM_COPPER_FURNACES);
Store copperCableAssemblyMachines("Assembly Machines", NUM_CABLE_ASSEMBLY_MACHINES);
Store electronicCircuitAssemblyMachines("Assembly Machines", NUM_ELECTRONIC_CIRCUIT_ASSEMBLY_MACHINES);


class SulfurProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions itself
    }
};

class SulfurProductionProcess : public Process {
    void Behavior() override {
        if (waterQueue.Length() >= MIN_WATER_FOR_SULFUR && petroleumGasForSulfurQueue.Length() >= MIN_GAS_FOR_SULFUR) {
            for (int i = 0; i < MIN_GAS_FOR_SULFUR; ++i) {
                auto *water = waterQueue.GetFirst();
                delete water;
                auto *petroleumGas = petroleumGasForSulfurQueue.GetFirst();
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
            if (Time + CH_PLANT_TIME <= SIMULATION_TIME && waterQueue.Length() >= MIN_WATER_FOR_SULFUR && petroleumGasForSulfurQueue.Length() >= MIN_GAS_FOR_SULFUR && sulfurChemicalPlant.Free() > 0) {
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
        if (crudeOilForSulfurQueue.Length() >= MIN_CRUDE_OIL_FOR_GAS) {
            for (int i = 0; i < MIN_CRUDE_OIL_FOR_GAS; ++i) {
                auto *crudeOil = crudeOilForSulfurQueue.GetFirst();
                delete crudeOil;
            }
            Enter(oilRefineryForSulfur);
            Wait(OIL_REFINING_TIME);
            Leave(oilRefineryForSulfur);
            for (int i = 0; i < PETROLEUM_GAS_BATCH; ++i) {
                auto *petroleumGas = new CrudeOilBatchProcess();
                petroleumGasForSulfurQueue.Insert(petroleumGas);
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
        Enter(pumpjackForSulfur);
        Wait(PUMPJACK_PROCESS_TIME);
        Leave(pumpjackForSulfur);
        for (int i = 0; i < MIN_CRUDE_OIL_FOR_GAS; ++i) {
            auto *oilBatch = new CrudeOilBatchProcess();
            crudeOilForSulfurQueue.Insert(oilBatch);
        }
        crude_oil_produced += CRUDE_OIL_BATCH;
        if (Time + PUMPJACK_PROCESS_TIME < SIMULATION_TIME && oilRefineryForSulfur.Free() > 0) {
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
        Enter(waterWellPump);
        Wait(WATER_PROCESS_TIME);
        Leave(waterWellPump);
        for (int i = 0; i < WATER_UNIT_BATCH; ++i) {
            auto *waterBatch = new WaterBatchProcess();
            waterQueue.Insert(waterBatch);
        }
        water_units_produced += WATER_UNIT_BATCH;
        if (Time + CH_PLANT_TIME <= SIMULATION_TIME && sulfurChemicalPlant.Free() > 0) {
            (new SulfurProductionProcess())->Activate();
        }
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
        if (!ironPlateQueueForCircuit.Empty() && copperCableForElectronicCircuit.Length() >= MIN_COPPER_CABLE_FOR_EL_CIRCUIT) {
            auto *plate = ironPlateQueueForCircuit.GetFirst(); // Take iron plate from conveyor 1
            delete plate;                             // Remove the plate from memory

            for (int i = 0; i < MIN_COPPER_CABLE_FOR_EL_CIRCUIT; ++i) {
                auto *cable = copperCableForElectronicCircuit.GetFirst(); // Take 3 copper cables from conveyor 1
                delete cable;                               // Remove the cable from memory
            }

            Enter(electronicCircuitAssemblyMachines);                   // Enter the store (seize an assembly machine)
            Wait(CIRCUIT_ASSEMBLY_TIME);                                 // Assembly takes 1 second
            Leave(electronicCircuitAssemblyMachines);                   // Leave the store (release the assembly machine)

            auto *circuit = new ElectronicCircuitProcess(); // Create a unique electronic circuit process
            electronicCircuitQueue.Insert(circuit);    // Insert the electronic circuit into the queue
            electronic_circuits_produced++;             // Increment the electronic circuits counter

            //std::cout << "Electronic circuit created and added to the queue. Total circuits: " << electronic_circuits_produced << "\n";
            //std::cout << "Processes in electronicCircuitQueue: " << electronicCircuitQueue.Length() << "\n";

            if (Time + CIRCUIT_ASSEMBLY_TIME <= SIMULATION_TIME && !ironPlateQueueForCircuit.Empty() && copperCableForElectronicCircuit.Length() >= MIN_COPPER_CABLE_FOR_EL_CIRCUIT) {
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
            for (int i = 0; i < COPPER_CABLE_BATCH; ++i) {              // Create 2 copper cables from 1 copper plate
                auto *cable = new CopperCableProcess(); // Create a unique cable process
                copperCableQueue.Insert(cable);        // Insert the copper cable into the queue
                copper_cables_produced++;               // Increment the copper cables counter
            }
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
            auto *ore = copperOreQueue.GetFirst();
            delete ore;
            Enter(copperFurnaces);
            Wait(COPPER_SMELTING_TIME);
            Leave(copperFurnaces);
            auto *plate = new CopperPlateProcess();
            copperPlateQueue.Insert(plate);
            copper_plates_produced++;
            if (Time + COPPER_SMELTING_TIME <= SIMULATION_TIME && !copperOreQueue.Empty()) {
                (new CopperSmeltingProcess())->Activate();
            }
            if (Time + CABLE_PRODUCTION_TIME <= SIMULATION_TIME && copperCableAssemblyMachines.Free() > 0 && !copperPlateQueue.Empty()) {
                (new CableProductionProcess())->Activate();
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
        Enter(ironOreDrills);
        Wait(IRON_MINING_TIME);
        Leave(ironOreDrills);
        auto *ore = new OreProcess();
        ironOreQueue.Insert(ore);
        iron_ore_produced++;
        if (Time + IRON_SMELTING_TIME <= SIMULATION_TIME && ironElectricalFurnaces.Free() > 0 && !ironOreQueue.Empty()) {
            (new IronSmeltingProcess())->Activate();
        }
    }
};

class CopperMiningProcess : public Process {
    void Behavior() override {
        Enter(copperMiningDrills);
        Wait(COPPER_MINING_TIME);
        Leave(copperMiningDrills);
        auto *ore = new CopperOreProcess();
        copperOreQueue.Insert(ore);
        copper_ore_produced++;
        if (Time + COPPER_SMELTING_TIME <= SIMULATION_TIME && copperFurnaces.Free() > 0 && !copperOreQueue.Empty()) {
            (new CopperSmeltingProcess())->Activate();
        }
    }
};

class OilProductionGenerator : public Event {
    void Behavior() override {
        (new PumpjackProcess())->Activate();
        if (Time + PUMPJACK_PROCESS_TIME < SIMULATION_TIME) {
            Activate(Time + PUMPJACK_PROCESS_TIME);
        }
    }
};

class WaterProductionGenerator : public Event {
    void Behavior() override {
        (new WaterProductionProcess())->Activate();
        if (Time + WATER_PROCESS_TIME < SIMULATION_TIME) {
            Activate(Time + WATER_PROCESS_TIME);
        }
    }
};

class PlasticProcess : public Process {
    void Behavior() override {}
};

class PlasticProductionProcess : public Process {
    void Behavior() override {
        if (petroleumGasForPlasticBarQueue.Length() >= MIN_GAS_FOR_PLASTIC_BAR && coalQueue.Length() >= MIN_COAL_FOR_PLASTIC_BAR) {
            for (int i = 0; i < MIN_GAS_FOR_PLASTIC_BAR; ++i) {
                auto *petroleumGas = petroleumGasForPlasticBarQueue.GetFirst();
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
            if (Time + CH_PLANT_TIME <= SIMULATION_TIME && petroleumGasForPlasticBarQueue.Length() >= 20 && coalQueue.Length() >= 1 && plasticChemicalPlant.Free() > 0) {
                (new PlasticProductionProcess())->Activate();
            }
        }
    }
};

class CrudeOilForPlasticBarBatchProcess : public Process {
    void Behavior() override {}
};

class OilRefiningForPlasticBarProcess : public Process {
    void Behavior() override {
        if (crudeOilForPlasticBarQueue.Length() >= MIN_CRUDE_OIL_FOR_GAS) {
            for (int i = 0; i < MIN_CRUDE_OIL_FOR_GAS; ++i) {
                auto *crudeOil = crudeOilForPlasticBarQueue.GetFirst();
                delete crudeOil;
            }
            Enter(oilRefineryForPlasticBar);
            Wait(OIL_REFINING_TIME);
            Leave(oilRefineryForPlasticBar);
            for (int i = 0; i < PETROLEUM_GAS_BATCH; ++i) {
                auto *petroleumGas = new CrudeOilForPlasticBarBatchProcess();
                petroleumGasForPlasticBarQueue.Insert(petroleumGas);
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
            crudeOilForPlasticBarQueue.Insert(oilBatch);
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
    void Behavior() override {}
};

class CoalMiningProcess : public Process {
    void Behavior() override {
        Enter(coalDrills);
        Wait(COAL_MINING_TIME);
        Leave(coalDrills);
        auto *coal = new CoalProcess();
        coalQueue.Insert(coal);
        coal_produced++;
        if (Time + COAL_MINING_TIME <= SIMULATION_TIME && coalDrills.Free() > 0) {
            (new CoalMiningProcess())->Activate();
        }
    }
};

class ResourceGenerator : public Event {
    void Behavior() override {
        for (int i = 0; i < NUM_IRON_ORE_DRILLS; ++i) {
            (new IronMiningProcess())->Activate();
        }
        for (int i = 0; i < NUM_COPPER_DRILLS; ++i) {
            (new CopperMiningProcess())->Activate();
        }
        for (int i = 0; i < NUM_COAL_DRILLS; ++i) {
            (new CoalMiningProcess())->Activate();
        }
        if (Time + IRON_MINING_TIME < SIMULATION_TIME) { // IRON_MINING_TIME == COPPER_MINING_TIME == COAL_MINING_TIME
            Activate(Time + IRON_MINING_TIME);
        }
    }
};

void best_values() {
    int iron_mine_ore = int(SIMULATION_TIME / IRON_MINING_TIME) * NUM_IRON_ORE_DRILLS;
    int iron_smelt_ore = int((SIMULATION_TIME - IRON_MINING_TIME) / IRON_SMELTING_TIME) * NUM_IRON_FURNACES;
    int copper_mine_ore = int(SIMULATION_TIME / COPPER_MINING_TIME) * NUM_COPPER_DRILLS;
    int copper_smelt_ore = int((SIMULATION_TIME - COPPER_MINING_TIME) / COPPER_SMELTING_TIME) * NUM_COPPER_FURNACES;
    int copper_cable = int((SIMULATION_TIME - COPPER_SMELTING_TIME - COPPER_MINING_TIME) / CABLE_PRODUCTION_TIME) * COPPER_CABLE_BATCH * NUM_CABLE_ASSEMBLY_MACHINES;
    int electronic_circuit = int((SIMULATION_TIME - COPPER_SMELTING_TIME - COPPER_MINING_TIME - CABLE_PRODUCTION_TIME) / CIRCUIT_ASSEMBLY_TIME) * NUM_ELECTRONIC_CIRCUIT_ASSEMBLY_MACHINES;
    int sulfur = int((SIMULATION_TIME - PUMPJACK_PROCESS_TIME-OIL_REFINING_TIME) / CH_PLANT_TIME) * NUM_SULFUR_PLANTS * SULFUR_BATCH ;
    int plastic = int((SIMULATION_TIME - PUMPJACK_PROCESS_TIME-OIL_REFINING_TIME) / CH_PLANT_TIME) * NUM_PLASTIC_PLANTS * PLASTIC_BAR_BATCH;
    int coal = int(SIMULATION_TIME / COAL_MINING_TIME) * NUM_COAL_DRILLS;
    int petroleum_gas_for_plastic = int((SIMULATION_TIME - PUMPJACK_PROCESS_TIME)/OIL_REFINING_TIME) * NUM_OIL_REFINERIES_FOR_PLASTIC_BAR * PETROLEUM_GAS_BATCH ;
    int petroleum_gas_for_sulfur = int((SIMULATION_TIME - PUMPJACK_PROCESS_TIME)/OIL_REFINING_TIME) * NUM_OIL_REFINERIES_FOR_SULFUR * PETROLEUM_GAS_BATCH ;
    std::cout << "Best values: \n";
    std::cout << "Mining iron ore: " << iron_mine_ore << "\n";
    std::cout << "Smelting iron ore: " << iron_smelt_ore << "\n";
    std::cout << "Mining copper ore: " << copper_mine_ore << "\n";
    std::cout << "Smelting copper ore: " << copper_smelt_ore << "\n";
    std::cout << "Copper cable production: " << copper_cable << "\n";
    std::cout << "Electronic circuit production: " << electronic_circuit << "\n";
    std::cout << "petroleum gas for plastic production: " << petroleum_gas_for_plastic << "\n";
    std::cout << "Coal production: " << coal << "\n";
    std::cout << "Plastic production: " << plastic << "\n";
    std::cout << "petroleum gas for sulfur production: " << petroleum_gas_for_sulfur << "\n";
    std::cout << "Sulfur production: " << sulfur << "\n";
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

//    std::cout << "Iron Ore mined during the simulation time: " << iron_ore_produced << "\n";
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
    std::cout << "Crude oil units left: " << crudeOilForPlasticBarQueue.Length() << "\n\n";
    pumpjackForPlasticBar.Output();

    std::cout << "Total petroleum gas for plastic produced: " << petroleum_gas_produced << " units.\n";
    std::cout << "Petroleum gas for plastic left: " << petroleumGasForPlasticBarQueue.Length() << "\n\n";
    oilRefineryForPlasticBar.Output();

    std::cout << "Total plastic bars produced: " << plastic_bar_produced << " units.\n";
    std::cout << "Plastic bar left: " << plasticBarQueue.Length() << "\n\n";

    std::cout << "Iron Ore mined: " << iron_ore_produced << "\n";
    std::cout << "Iron Plates created: " << iron_plates_produced << "\n";
    std::cout << "Copper Ore mined: " << copper_ore_produced << "\n";
    std::cout << "Copper Plates created: " << copper_plates_produced << "\n";
    std::cout << "Copper Cables created: " << copper_cables_produced << "\n";
    std::cout << "Copper cables for El.circuit: " << copperCableForElectronicCircuit.Length() << "\n";
    std::cout << "Copper cables for Adv.El.circuit: " << copperCableForAdvanceElectronicCircuit.Length() << "\n";
    std::cout << "Iron plates for Sulfuric Acid: " << ironPlateQueueForSulfur.Length() << "\n";
    std::cout << "Iron plates for El.circuit: " << ironPlateQueueForCircuit.Length() << "\n";
    std::cout << "Electronic circuits created: " << electronic_circuits_produced << "\n";
    std::cout << "Electronic circuit queue: " << electronicCircuitQueue.Length() << "\n";

    best_values();

    return 0;
}