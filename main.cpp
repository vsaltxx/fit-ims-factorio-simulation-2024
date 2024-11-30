#include "simlib.h"
#include <iostream>

// Constants
const double ARRIVAL_INTERVAL = 5.0;   // Interval for resource arrivals
const double IRON_PLATE_TIME = 3.5;    // Processing time for iron ore
const double COPPER_PLATE_TIME = 3.5;  // Processing time for copper ore
const double CIRCUIT_TIME = 0.5;       // Assembly time for electronic circuits
const double ADV_CIRCUIT_TIME = 6.0;   // Assembly time for advanced circuits
const double PROCESSOR_TIME = 10.0;    // Assembly time for a processor
const int TOTAL_PROCESSORS = 10;       // Total number of processors to assemble

// Global variables
int processors_created = 0;            // Counter for assembled processors

// Resource queues
Queue ironOreQueue("Iron Ore Queue");
Queue copperOreQueue("Copper Ore Queue");
Queue ironPlateQueue("Iron Plate Queue");
Queue copperPlateQueue("Copper Plate Queue");
Queue circuitQueue("Electronic Circuit Queue");
Queue advCircuitQueue("Advanced Circuit Queue");

// Facilities
Facility ironSmelter("Iron Smelter");
Facility copperSmelter("Copper Smelter");
Facility circuitAssembler("Electronic Circuit Assembler");
Facility advCircuitAssembler("Advanced Circuit Assembler");
Facility processorAssembler("Processor Assembler");


// Process class: Processor Assembly
class ProcessorProcess : public Process {
    void Behavior() override {
        if (!advCircuitQueue.Empty() && !circuitQueue.Empty()) {
            std::cout << "Activating ProcessorProcess\n";
            advCircuitQueue.GetFirst();
            circuitQueue.GetFirst();
            Seize(processorAssembler);
            Wait(PROCESSOR_TIME);
            Release(processorAssembler);
            processors_created++;
            std::cout << "Processor assembled. Total: " << processors_created << "\n";
        } else {
            std::cout << "ProcessorProcess conditions not met\n";
        }
    }
};

// Process class: Advanced Circuit Assembly
class AdvCircuitProcess : public Process {
    void Behavior() override {
        if (!circuitQueue.Empty() && !copperPlateQueue.Empty()) {
            std::cout << "Activating AdvCircuitProcess\n";
            circuitQueue.GetFirst();
            copperPlateQueue.GetFirst();
            Seize(advCircuitAssembler);
            Wait(ADV_CIRCUIT_TIME);
            Release(advCircuitAssembler);
            (new ProcessorProcess())->Activate(); // Activate processor assembly
        } else {
            std::cout << "AdvCircuitProcess conditions not met\n";
        }
    }
};


// Process class: Electronic Circuit Assembly
class CircuitProcess : public Process {
    void Behavior() override {
        if (!ironPlateQueue.Empty() && !copperPlateQueue.Empty()) {
            std::cout << "Activating CircuitProcess\n";
            ironPlateQueue.GetFirst();
            copperPlateQueue.GetFirst();
            Seize(circuitAssembler);
            Wait(CIRCUIT_TIME);
            Release(circuitAssembler);
            (new AdvCircuitProcess())->Activate(); // Activate advanced circuit assembly
        } else {
            std::cout << "CircuitProcess conditions not met\n";
        }
    }
};

// Function to check and activate electronic circuit assembly
void CheckAndActivateCircuitProcess() {
    if (!ironPlateQueue.Empty() && !copperPlateQueue.Empty()) {
        (new class CircuitProcess())->Activate(); // Activate circuit assembly process
    }
}

// Process class: Iron Plate Production
class IronPlateProcess : public Process {
    void Behavior() override {
        if (!ironOreQueue.Empty()) {
            ironOreQueue.GetFirst();
            Seize(ironSmelter);
            Wait(IRON_PLATE_TIME);
            Release(ironSmelter);
            ironPlateQueue.Insert(this);
            CheckAndActivateCircuitProcess();
        }
    }
};

// Process class: Copper Plate Production
class CopperPlateProcess : public Process {
    void Behavior() override {
        if (!copperOreQueue.Empty()) {
            copperOreQueue.GetFirst();
            Seize(copperSmelter);
            Wait(COPPER_PLATE_TIME);
            Release(copperSmelter);
            copperPlateQueue.Insert(this);
            CheckAndActivateCircuitProcess();
        }
    }
};

// Event class: Resource Generation
class ResourceGenerator : public Event {
    void Behavior() override {
        auto *ironProcess = new IronPlateProcess();
        ironOreQueue.Insert(ironProcess);
        ironProcess->Activate();

        auto *copperProcess = new CopperPlateProcess();
        copperOreQueue.Insert(copperProcess);
        copperProcess->Activate();

        if (processors_created < TOTAL_PROCESSORS) {
            Activate(Time + ARRIVAL_INTERVAL);
        }
    }
};

int main() {
    // Initialize simulation
    Init(0, 1000);

    // Start resource generation
    (new ResourceGenerator())->Activate();

    // Run the simulation
    Run();

    // Results
    std::cout << "Processors assembled: " << processors_created << "\n";
    ironSmelter.Output();
    copperSmelter.Output();
    circuitAssembler.Output();
    advCircuitAssembler.Output();
    processorAssembler.Output();

    return 0;
}
