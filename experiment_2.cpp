#include "simlib.h"
#include <iostream>

// Constants
const double SIMULATION_TIME = 133;// Total simulation time in seconds

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
const double SULFURIC_ACID_PRODUCTION_TIME = 1.0;
const double ADVANCED_CIRCUIT_PRODUCTION_TIME = 12.0;
const double PROCESSOR_PRODUCTION_TIME = 7.5;

const int WATER_UNIT_BATCH = 1200;
const int CRUDE_OIL_BATCH = 100;
const int PETROLEUM_GAS_BATCH = 45;
const int SULFUR_BATCH = 2;
const int PLASTIC_BAR_BATCH = 2;
const int COPPER_CABLE_BATCH = 2;
const int SULFURIC_ACID_BATCH = 50;
const int ADVANCED_CIRCUIT_BATCH = 1;
const int PROCESSOR_BATCH = 1;

const int  MIN_CRUDE_OIL_FOR_GAS = 100;
const int  MIN_WATER_FOR_SULFUR = 30;
const int  MIN_GAS_FOR_SULFUR = 30;
const int  MIN_GAS_FOR_PLASTIC_BAR = 20;
const int  MIN_COAL_FOR_PLASTIC_BAR = 1;
const int  MIN_COPPER_CABLE_FOR_EL_CIRCUIT = 3;
const int  MIN_EL_CIRCUITS_FOR_PROCESSOR = 20;
const int  MIN_ADV_EL_CIRCUITS_FOR_PROCESSOR = 2;
const int  MIN_SULFURIC_ACID_FOR_PROCESSOR = 5;
const int  MIN_WATER_FOR_SULFURIC_ACID = 100;
const int  MIN_SULFUR_FOR_SULFURIC_ACID = 5;
const int  MIN_IRON_PLATE_FOR_SULFURIC_ACID = 1;
const int  MIN_EL_CIRCUIT_FOR_ADV_CIRCUIT = 2;
const int  MIN_COPPER_CABLE_FOR_ADV_CIRCUIT = 4;
const int  MIN_PLASTIC_BAR_FOR_ADV_CIRCUIT = 2;

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
const int NUM_SULFURIC_ACID_CHEMICAL_PLANTS = 1;
const int NUM_ADVANCED_CIRCUIT_ASSEMBLERS = 1;
const int NUM_PROCESSOR_ASSEMBLERS = 1;

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
int sulfuric_acid_produced = 0;
int advanced_electronic_circuits_produced = 0;
int processors_produced = 0;

bool CopperToggleSplitter = true;
bool IronToggleSplitter = true;
bool ElectronicCircuitToggleSplitter = true;
bool IronPlateForSulfuricSplitterToggle = true;
bool CopperCableSplitterToggle = true;
bool ElectronicCircuitSplitterToggle = true;

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
Queue copperCableForAdvancedElectronicCircuit("Copper Cable Queue 2 (For Advance Electronic Circuit)");
Queue copperCableForAdvancedElectronicCircuitAfterSplit("Copper Cable Queue 2 (For Advance Electronic Circuit)");
Queue ironPlateQueueForCircuit("Iron Plate Queue 2 (For Electronic Circuit)");
Queue ironPlateQueueForSulfur("Iron Plate Queue 1 (For Sulfuric Acid)");
Queue ironPlateQueueForSulfurAfterSplit("Iron Plate Queue 1 (For Sulfuric Acid)");
Queue electronicCircuitQueue("Electronic Circuit Queue");
Queue electronicCircuitQueueForAdv("Electronic Circuit Queue 2 (For Advance Electronic Circuit)");
Queue electronicCircuitQueueForAdvAfterSplit("Electronic Circuit Queue 2 (For Advance Electronic Circuit)");
Queue electronicCircuitQueueForProcessingUnit("Electronic Circuit Queue 1 (For Electronic Circuit)");
Queue sulfuricAcidQueue("sulfuricAcid Queue");
Queue advancedElectronicCircuitQueue("Advanced Electronic Circuit Queue");
Queue processorQueue("Processor Queue");

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
Store sulfuricAcidChemicalPlant("Liquid Chemical Plant", NUM_SULFURIC_ACID_CHEMICAL_PLANTS);
Store advancedCircuitAssembler("Advanced Circuit Assembler", NUM_ADVANCED_CIRCUIT_ASSEMBLERS);
Store processorAssembler("Processor Assembler", NUM_PROCESSOR_ASSEMBLERS);


class SulfurProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions itself
    }
};

class SulfuricAcidProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions itself
    }
};

class AdvancedElectronicCircuitProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions itself
    }
};

class ProcessorProcess : public Process {
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

class ProcessorProductionProcess : public Process {
    void Behavior() override {
        if (electronicCircuitQueueForProcessingUnit.Length() >= MIN_EL_CIRCUITS_FOR_PROCESSOR && advancedElectronicCircuitQueue.Length() >= MIN_ADV_EL_CIRCUITS_FOR_PROCESSOR && sulfuricAcidQueue.Length() >= MIN_SULFURIC_ACID_FOR_PROCESSOR) {
            for (int i = 0; i < MIN_EL_CIRCUITS_FOR_PROCESSOR; ++i) {
                auto *circuit = electronicCircuitQueueForProcessingUnit.GetFirst();
                delete circuit;
            }
            for (int i = 0; i < MIN_ADV_EL_CIRCUITS_FOR_PROCESSOR; ++i) {
                auto *advancedCircuit = advancedElectronicCircuitQueue.GetFirst();
                delete advancedCircuit;
            }
            for (int i = 0; i < MIN_SULFURIC_ACID_FOR_PROCESSOR; ++i) {
                auto *sulfuricAcid = sulfuricAcidQueue.GetFirst();
                delete sulfuricAcid;
            }
            Enter(processorAssembler);
            Wait(PROCESSOR_PRODUCTION_TIME);
            Leave(processorAssembler);
            auto *processor = new ProcessorProcess();
            processorQueue.Insert(processor);
            processors_produced += PROCESSOR_BATCH;
            if (Time + PROCESSOR_PRODUCTION_TIME <= SIMULATION_TIME && electronicCircuitQueueForProcessingUnit.Length() >= MIN_EL_CIRCUITS_FOR_PROCESSOR && advancedElectronicCircuitQueue.Length() >= MIN_ADV_EL_CIRCUITS_FOR_PROCESSOR && sulfuricAcidQueue.Length() >= MIN_SULFURIC_ACID_FOR_PROCESSOR && processorAssembler.Free() > 0) {
                (new ProcessorProductionProcess())->Activate();
            }
        }
    }
};

class CrudeOilBatchProcess  : public Process {
    void Behavior() override {}
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

class SulfuricAcidProductionProcess : public Process {
    void Behavior() override {
        if (waterQueue.Length() >= MIN_WATER_FOR_SULFURIC_ACID && ironPlateQueueForSulfurAfterSplit.Length() >= MIN_IRON_PLATE_FOR_SULFURIC_ACID && sulfurQueue.Length() >= MIN_SULFUR_FOR_SULFURIC_ACID) {
            for (int i = 0; i < MIN_WATER_FOR_SULFURIC_ACID; ++i) {
                auto *water = waterQueue.GetFirst();
                delete water;
            }
            for (int i = 0; i < MIN_IRON_PLATE_FOR_SULFURIC_ACID; ++i) {
                auto *ironPlate = ironPlateQueueForSulfurAfterSplit.GetFirst();
                delete ironPlate;
            }
            for (int i = 0; i < MIN_SULFUR_FOR_SULFURIC_ACID; ++i) {
                auto *sulfur = sulfurQueue.GetFirst();
                delete sulfur;
            }
            Enter(sulfurChemicalPlant);
            Wait(SULFURIC_ACID_PRODUCTION_TIME);
            Leave(sulfurChemicalPlant);
            for (int i = 0; i < SULFURIC_ACID_BATCH; ++i) {
                auto *liquid = new SulfuricAcidProcess();
                sulfuricAcidQueue.Insert(liquid);
            }
            sulfuric_acid_produced += SULFURIC_ACID_BATCH;
            if (Time + SULFURIC_ACID_PRODUCTION_TIME <= SIMULATION_TIME && waterQueue.Length() >= MIN_WATER_FOR_SULFURIC_ACID && ironPlateQueue.Length() >= MIN_IRON_PLATE_FOR_SULFURIC_ACID && sulfurQueue.Length() >= MIN_SULFUR_FOR_SULFURIC_ACID && sulfuricAcidChemicalPlant.Free() > 0) {
                (new SulfuricAcidProductionProcess())->Activate();
            }
        }
    }
};

class AdvancedElectronicCircuitProductionProcess : public Process {
    void Behavior() override {
        if (electronicCircuitQueueForAdvAfterSplit.Length() >= MIN_EL_CIRCUIT_FOR_ADV_CIRCUIT && plasticBarQueue.Length() >= MIN_PLASTIC_BAR_FOR_ADV_CIRCUIT && copperCableForAdvancedElectronicCircuitAfterSplit.Length() >= MIN_COPPER_CABLE_FOR_ADV_CIRCUIT) {
            for (int i = 0; i < MIN_EL_CIRCUIT_FOR_ADV_CIRCUIT; ++i) {
                auto *circuit = electronicCircuitQueueForAdvAfterSplit.GetFirst();
                delete circuit;
            }
            for (int i = 0; i < MIN_PLASTIC_BAR_FOR_ADV_CIRCUIT; ++i) {
                auto *plastic = plasticBarQueue.GetFirst();
                delete plastic;
            }
            for (int i = 0; i < MIN_COPPER_CABLE_FOR_ADV_CIRCUIT; ++i) {
                auto *cable = copperCableForAdvancedElectronicCircuitAfterSplit.GetFirst();
                delete cable;
            }
            Enter(advancedCircuitAssembler);
            Wait(ADVANCED_CIRCUIT_PRODUCTION_TIME);
            Leave(advancedCircuitAssembler);

            auto *advancedCircuit = new AdvancedElectronicCircuitProcess();
            advancedElectronicCircuitQueue.Insert(advancedCircuit);
            advanced_electronic_circuits_produced += ADVANCED_CIRCUIT_BATCH;
            if (Time + ADVANCED_CIRCUIT_PRODUCTION_TIME <= SIMULATION_TIME && electronicCircuitQueueForAdvAfterSplit.Length() >= MIN_EL_CIRCUIT_FOR_ADV_CIRCUIT && plasticBarQueue.Length() >= MIN_PLASTIC_BAR_FOR_ADV_CIRCUIT && copperCableForAdvancedElectronicCircuitAfterSplit.Length() >= MIN_COPPER_CABLE_FOR_EL_CIRCUIT && advancedCircuitAssembler.Free() > 0) {
                (new AdvancedElectronicCircuitProductionProcess())->Activate();
            }
        }
    }
};

class ElectronicCircuitSplitterForAdvancedCircuits : public Process {
    void Behavior() override {
        while (1) {
            if (!electronicCircuitQueueForAdv.Empty()) {
                auto *circuit = electronicCircuitQueueForAdv.GetFirst(); // Take electronic circuit from the queue

                if (Time + ADVANCED_CIRCUIT_PRODUCTION_TIME <= SIMULATION_TIME && !ElectronicCircuitSplitterToggle) {
                    electronicCircuitQueueForAdvAfterSplit.Insert(circuit); // Insert into the queue for advanced electronic circuits
                    (new AdvancedElectronicCircuitProductionProcess())->Activate();
                } else {
                    electronicCircuitQueueForProcessingUnit.Insert(circuit); // Insert into the queue for processors
                    (new ProcessorProductionProcess())->Activate();
                }

                ElectronicCircuitSplitterToggle = !ElectronicCircuitSplitterToggle; // Toggle the flag for the next circuit
            } else {
                Passivate(); // Wait until a circuit appears in the queue
            }
        }
    }
};

class ElectronicCircuitSplitter : public Process {
    void Behavior() override {
        while (1) {
            if (!electronicCircuitQueue.Empty()) {
                auto *circuit = electronicCircuitQueue.GetFirst(); // Take electronic circuit from the queue
                if (!ElectronicCircuitToggleSplitter) {
                    // Insert into the first queue for processors
                    electronicCircuitQueueForProcessingUnit.Insert(circuit);
                    (new ProcessorProductionProcess())->Activate();
                } else {
                    // Insert into the second queue for advanced electronic circuits
                    electronicCircuitQueueForAdv.Insert(circuit);
                    (new ElectronicCircuitSplitterForAdvancedCircuits())->Activate();
                }
                ElectronicCircuitToggleSplitter = !ElectronicCircuitToggleSplitter; // Toggle the flag for the next circuit
            } else {
                Passivate(); // Wait until a circuit appears in the queue
            }
        }
    }
};

class WaterBatchProcess : public Process {
    void Behavior() override {}
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
            auto *plate = ironPlateQueueForCircuit.GetFirst();              // Take iron plate from conveyor 1
            delete plate;
            for (int i = 0; i < MIN_COPPER_CABLE_FOR_EL_CIRCUIT; ++i) {
                auto *cable = copperCableForElectronicCircuit.GetFirst();   // Take 3 copper cables from conveyor 1
                delete cable;
            }
            Enter(electronicCircuitAssemblyMachines);
            Wait(CIRCUIT_ASSEMBLY_TIME);
            Leave(electronicCircuitAssemblyMachines);
            auto *circuit = new ElectronicCircuitProcess();
            electronicCircuitQueue.Insert(circuit);
            electronic_circuits_produced++;
            if (Time + CIRCUIT_ASSEMBLY_TIME <= SIMULATION_TIME && !ironPlateQueueForCircuit.Empty() && copperCableForElectronicCircuit.Length() >= MIN_COPPER_CABLE_FOR_EL_CIRCUIT) {
                (new CircuitAssemblyProcess())->Activate();
            }
            if (!electronicCircuitQueue.Empty()) {
                (new ElectronicCircuitSplitter())->Activate(); // Activate the splitter process
            }
        }
    }
};

class IronPlateSplitterForSulfur : public Process {
    void Behavior() override {
        while (1) {
            if (!ironPlateQueueForSulfur.Empty()) {
                auto *plate = ironPlateQueueForSulfur.GetFirst(); // Take iron plate from the queue

                if (!IronPlateForSulfuricSplitterToggle) {

                    ironPlateQueueForSulfurAfterSplit.Insert(plate);// Insert into the queue for electronic circuits
                } else {
                    ironPlateQueueForCircuit.Insert(plate);// Insert into the queue for sulfuric acid
                }

                IronPlateForSulfuricSplitterToggle  = !IronPlateForSulfuricSplitterToggle; // Toggle the flag for the next plate
            } else {
                Passivate(); // Wait until a plate appears in the queue
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
                    (new IronPlateSplitterForSulfur())->Activate();
                    (new SulfuricAcidProductionProcess())->Activate();
                } else {
                    ironPlateQueueForCircuit.Insert(plate); // Insert into conveyor 2
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
            auto *ore = ironOreQueue.GetFirst();
            delete ore;
            Enter(ironElectricalFurnaces);
            Wait(IRON_SMELTING_TIME);
            Leave(ironElectricalFurnaces);
            auto *plate = new PlateProcess();
            ironPlateQueue.Insert(plate);
            iron_plates_produced++;
            if (!ironPlateQueue.Empty()) {
                (new IronPlateSplitter())->Activate();
            }
            if (Time + IRON_SMELTING_TIME <= SIMULATION_TIME && !ironOreQueue.Empty()) {
                (new IronSmeltingProcess())->Activate();
            }
        }
    }
};

class CopperCableSplitterForAdvancedCircuits : public Process {
    void Behavior() override {
        while (1) {
            if (!copperCableForAdvancedElectronicCircuit.Empty()) {
                auto *cable = copperCableForAdvancedElectronicCircuit.GetFirst(); // Take copper cable from the queue

                if (!CopperCableSplitterToggle) {
                    copperCableForAdvancedElectronicCircuitAfterSplit.Insert(cable); // Insert into the queue for advanced electronic circuits
                    (new AdvancedElectronicCircuitProductionProcess())->Activate();
                } else {
                    copperCableForElectronicCircuit.Insert(cable); // Insert into the queue for electronic circuits
                }

                CopperCableSplitterToggle = !CopperCableSplitterToggle; // Toggle the flag for the next cable
            } else {
                Passivate(); // Wait until a cable appears in the queue
            }
        }
    }
};

class CopperCableSplitter : public Process {
    void Behavior() override {
        while (1) {
            if (!copperCableQueue.Empty()) {
                auto *cable = copperCableQueue.GetFirst();
                if (!CopperToggleSplitter) {
                    copperCableForAdvancedElectronicCircuit.Insert(cable);
                    (new CopperCableSplitterForAdvancedCircuits())->Activate();
                } else {
                    copperCableForElectronicCircuit.Insert(cable);
                    (new CircuitAssemblyProcess())->Activate();
                }
                CopperToggleSplitter = !CopperToggleSplitter;
            } else {
                Passivate();
            }
        }
    }
};

class CableProductionProcess : public Process {
    void Behavior() override {
        if (!copperPlateQueue.Empty()) {
            auto *plate = copperPlateQueue.GetFirst();
            delete plate;
            Enter(copperCableAssemblyMachines);
            Wait(CABLE_PRODUCTION_TIME);
            Leave(copperCableAssemblyMachines);
            for (int i = 0; i < COPPER_CABLE_BATCH; ++i) {
                auto *cable = new CopperCableProcess();
                copperCableQueue.Insert(cable);
                copper_cables_produced++;
            }
            if (Time + CABLE_PRODUCTION_TIME <= SIMULATION_TIME && !copperPlateQueue.Empty()) {
                (new CableProductionProcess())->Activate();
            }
            if (!copperCableQueue.Empty()) {
                (new CopperCableSplitter())->Activate();
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
            if (Time + CH_PLANT_TIME <= SIMULATION_TIME && petroleumGasForPlasticBarQueue.Length() >= MIN_GAS_FOR_PLASTIC_BAR && coalQueue.Length() >= MIN_COAL_FOR_PLASTIC_BAR && plasticChemicalPlant.Free() > 0) {
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

// TODO: add comments to this function
void best_values() {
    int iron_mine_ore = int(SIMULATION_TIME / IRON_MINING_TIME) * NUM_IRON_ORE_DRILLS;
    int iron_smelt_ore = int((SIMULATION_TIME - IRON_MINING_TIME) / IRON_SMELTING_TIME) * NUM_IRON_FURNACES;
    int copper_mine_ore = int(SIMULATION_TIME / COPPER_MINING_TIME) * NUM_COPPER_DRILLS;
    int copper_smelt_ore = int((SIMULATION_TIME - COPPER_MINING_TIME) / COPPER_SMELTING_TIME) * NUM_COPPER_FURNACES;
    int copper_cable = int((SIMULATION_TIME - COPPER_SMELTING_TIME - COPPER_MINING_TIME) / CABLE_PRODUCTION_TIME) * COPPER_CABLE_BATCH * NUM_CABLE_ASSEMBLY_MACHINES;
    int electronic_circuit = int((SIMULATION_TIME - COPPER_SMELTING_TIME - COPPER_MINING_TIME - CABLE_PRODUCTION_TIME) / CIRCUIT_ASSEMBLY_TIME) * NUM_ELECTRONIC_CIRCUIT_ASSEMBLY_MACHINES;
    int sulfur = int((SIMULATION_TIME - PUMPJACK_PROCESS_TIME - OIL_REFINING_TIME) / CH_PLANT_TIME) * NUM_SULFUR_PLANTS * SULFUR_BATCH ;
    int plastic = int((SIMULATION_TIME - PUMPJACK_PROCESS_TIME - OIL_REFINING_TIME) / CH_PLANT_TIME) * NUM_PLASTIC_PLANTS * PLASTIC_BAR_BATCH;
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
    std::cout << "Simulation finished \n \n";
    std::cout << "copper plates produced: " << copper_plates_produced << "\n";
    std::cout << "Copper cables  produced: " << copper_cables_produced << "\n";
    std::cout << "Electronic circuits produced: " << electronic_circuits_produced << "\n";
    // Print all queues
    std::cout << "Iron ore: " << ironOreQueue.Length() << "\n";
    std::cout << "Iron plates: " << ironPlateQueue.Length() << "\n";
    std::cout << "Iron plates for Sulfuric Acid: " << ironPlateQueueForSulfurAfterSplit.Length() << "\n";
    std::cout << "Iron plates for El.circuit: " << ironPlateQueueForCircuit.Length() << "\n";
    std::cout << "Copper ore: " << copperOreQueue.Length() << "\n";
    std::cout << "Copper plates: " << copperPlateQueue.Length() << "\n";
    std::cout << "Copper cables: " << copperCableQueue.Length() << "\n";
    std::cout << "Copper cables for El.circuit: " << copperCableForElectronicCircuit.Length() << "\n";
    std::cout << "Copper cables for Adv.El.circuit: " << copperCableForAdvancedElectronicCircuitAfterSplit.Length() << "\n";

    std::cout << "Electronic circuit queue: " << electronicCircuitQueue.Length() << "\n";
    std::cout << "Electronic circuit for Adv.El.circuit: " << electronicCircuitQueueForAdvAfterSplit.Length() << "\n";
    std::cout << "Electronic circuit for Processor: " << electronicCircuitQueueForProcessingUnit.Length() << "\n";

    std::cout << "Coal: " << coalQueue.Length() << "\n";
    std::cout << "Crude oil for plastic bar: " << crudeOilForPlasticBarQueue.Length() << "\n";
    std::cout << "Petroleum gas for plastic bar: " << petroleumGasForPlasticBarQueue.Length() << "\n";
    std::cout << "Plastic bars: " << plasticBarQueue.Length() << "\n";

    std::cout << "Advanced electronic circuits created: " << advancedElectronicCircuitQueue.Length() << "\n";

    std::cout << "Water units: " << waterQueue.Length() << "\n";
    std::cout << "Crude oil for sulfur: " << crudeOilForSulfurQueue.Length() << "\n";
    std::cout << "Petroleum gas for sulfur: " << petroleumGasForSulfurQueue.Length() << "\n";
    std::cout << "Sulfur: " << sulfurQueue.Length() << "\n";
    std::cout << "Sulfuric acid: " << sulfuricAcidQueue.Length() << "\n";

    std::cout << "Processors: " << processorQueue.Length() << "\n";

    return 0;
}