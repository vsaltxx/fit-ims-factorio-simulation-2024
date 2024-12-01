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

// Queue
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

class OreProcess : public Process {
    void Behavior() override {
        // This process doesn't perform any actions
    }
};

class MiningProcess : public Process {
    void Behavior() override {
        Enter(ironOreDrills);       // Enter the store (seize an iron ore mining drill)
        Wait(MINING_TIME);          // Mining takes 2 seconds
        Leave(ironOreDrills);       // Leave the store (release the iron ore mining drill)

        auto *ore = new OreProcess(); // Create a unique ore process
        ironOreQueue.Insert(ore);     // Insert mined iron ore into the queue
        iron_ore_produced++;             // Increment mined iron ore counter

        if (Time + SMELTING_TIME <= SIMULATION_TIME && electricalFurnaces.Free() > 0 && !ironOreQueue.Empty()) {
            (new SmeltingProcess())->Activate();
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
        // Activate multiple mining processes to utilize all drills
        for (int i = 0; i < NUM_IRON_ORE_DRILLS; ++i) {
            (new MiningProcess())->Activate();
        }
        for (int i = 0; i < NUM_COAL_DRILLS; ++i) {
            (new CoalMiningProcess())->Activate();
        }

        // Continue generating resources until the simulation time ends
        if (Time + MINING_TIME < SIMULATION_TIME) {
            Activate(Time + MINING_TIME); // Schedule the next mining event
        }
    }
};

void best_values(){

    int mine_ore = SIMULATION_TIME / MINING_TIME * NUM_IRON_ORE_DRILLS;
    int smelt_ore = (SIMULATION_TIME-MINING_TIME) /SMELTING_TIME * NUM_FURNACES;
    int water = SIMULATION_TIME * WATER_UNIT_BATCH * NUM_WATER_PUMPS;
    std::cout << "Best values: \n";
    std::cout << "Mining ore: " << mine_ore << "\n";
    std::cout << "Smelting ore: " << smelt_ore << "\n";
    std::cout << "Water: " << water << "\n";
}

int main() {
    // Initialize simulation
    Init(0, SIMULATION_TIME);

    // Start the resource generation process
    (new OilProductionGenerator())->Activate();
    (new WaterProductionGenerator())->Activate();
    (new ResourceGenerator())->Activate();
    (new OilProductionForPlasticBarGenerator())->Activate();

    // Run the simulation
    Run();

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


    //electricalFurnaces.Output();
    //waterWellPump.Output();
    //best_values();

    return 0;
}