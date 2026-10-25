#include <cassert>
#include <iostream>
#include <TestScheduler.h>
#include <TestSimulator.h>

int main() {
    const int execution_time = 3;
    const int period = 10;

    assert(execution_time > 0);
    assert(period >= execution_time);

    std::cout << "All basic tests passed!\n";

	TestScheduler::TestEDFScheduler();
	TestScheduler::TestRMSScheduler();
    TestScheduler::TestFPPSScheduler();

    TestSimulator::RunSimulatorTest();
    TestSimulator::RunSimulatorSwitchTest();

    //wait for input before closing
    std::cin.get();
    return 0;
} 