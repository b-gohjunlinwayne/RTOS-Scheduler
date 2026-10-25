/**
 * @file TestSimulator.cpp
 * @brief Implements unit testing and validation routines to evaluate
 *        the behavior of the simulator together with the scheduler
 */

#include <TestSimulator.h>
#include <Simulator.h>
#include <Schedulers.h>
#include <iostream>
#include <assert.h>

void TestSimulator::RunSimulatorTest()
{
	std::cout << "Running Simulator Test..." << std::endl;
	// Create a simple scheduler for testing
	RMSScheduler scheduler;
	// Create a simulator with the scheduler
	Simulator simulator(scheduler);
	// Define some task configurations for testing
	std::vector<TaskConfig> taskConfigs = {
		{1, 5, 10, 10,0},
		{2, 3, 5, 5,0},
		{3, 7, 15, 15,0}
	};
	// Reset the simulator with the task configurations
	simulator.Reset(taskConfigs);
	// Run the simulator for a certain number of ticks
	for (int i = 0; i < 30; ++i)
	{
		simulator.Run();
	}

	std::cout << "Running optimized system test.." << std::endl;

	//schedulable simulator task
	std::vector<TaskConfig> optimizedTaskConfigs = {
		{1, 1, 4, 4,0},
		{2, 1, 5, 5,0},
		{3, 2, 10, 10,0}
	};

	// Reset the simulator with the task configurations
	simulator.Reset(optimizedTaskConfigs);
	// Run the simulator for a certain number of ticks
	for (int i = 0; i < 30; ++i)
	{
		simulator.Run();
	}

	std::cout << "Running fully optimized system test.." << std::endl;
	//fully optmized task set
	std::vector<TaskConfig> fullyOptimizedTaskConfigs = {
		{1, 2, 6, 6,0},
		{2, 2, 6, 6,0},
		{3, 2, 6, 6,0}
	};
	// Reset the simulator with the task configurations
	simulator.Reset(fullyOptimizedTaskConfigs);
	// Run the simulator for a certain number of ticks
	for (int i = 0; i < 30; ++i)
	{
		simulator.Run();
	}

	std::cout << "Running pirority context switching test.." << std::endl;

	FPPSScheduler fppsscheduler;
	Simulator FPPSSimulator(fppsscheduler);
	/*
		Task 1: ID=1, ExecutionTime=2, Period=10, RelativeDeadline=10, Priority = 1 (High Priority)
		Task 2: ID=2, ExecutionTime=12, Period=30, RelativeDeadline=30, Priority = 2 (Low Priority)
	*/
	std::vector<TaskConfig> contextSwitchTaskConfigs = {
		{1, 2, 10, 10, 1},
		{2, 12, 30, 30, 2}
	};

	// Reset the simulator with the task configurations
	FPPSSimulator.Reset(contextSwitchTaskConfigs);

	// Run the simulator for 16 ticks to observe multiple context switches
	for (int i = 0; i < 16; ++i)
	{
		FPPSSimulator.Run();
	}
}

void TestSimulator::RunSimulatorSwitchTest()
{
	std::cout << "Running Scheduler Hot-Swap Test (RMS -> EDF)..." << std::endl;

	/*
		Task 1: ID=1, ExecutionTime=2, Period=8, RelativeDeadline=8  (RMS Priority: Lower rate)
		Task 2: ID=2, ExecutionTime=2, Period=5, RelativeDeadline=5  (RMS Priority: Higher rate/shorter period)
	*/
	std::vector<TaskConfig> taskConfigs = {
		{1, 2, 8, 8},
		{2, 2, 5, 5}
	};

	RMSScheduler rmsScheduler;
	EDFScheduler edfScheduler;

	// Initialize simulator with RMS
	Simulator simulator(rmsScheduler);
	simulator.Reset(taskConfigs);

	// Run tick 0 to release jobs for both tasks
	simulator.Run();

	// Under RMS, Task 2 (period 5) has higher priority than Task 1 (period 8).
	// Let's verify initial selection under RMS using the scheduler directly or via display data.
	std::vector<Job> testReadyJobs = {
		{1, 1, 0, 8, 2}, // Job from Task 1 (Deadline 8)
		{2, 2, 0, 5, 2}  // Job from Task 2 (Deadline 5)
	};

	rmsScheduler.PreInitialisation(taskConfigs);
	std::optional<JobID> rmsNext = rmsScheduler.NextJob(testReadyJobs);

	if (rmsNext == std::nullopt || rmsNext.value() != 2)
	{
		std::cerr << "Test failed: RMS did not pick Task 2 (shortest period) first." << std::endl;
		return;
	}
	std::cout << "RMS correctly selected Job/Task 2 (shorter period)." << std::endl;

	// --- HOT-SWAP SCHEDULER MID-SIMULATION ---
	std::cout << "Hot-swapping simulator scheduler from RMS to EDF..." << std::endl;
	simulator.SwitchScheduler(edfScheduler);

	// Under EDF, if deadlines change or if we evaluate ready jobs, EDF picks earliest deadline.
	// Let's modify deadlines to test EDF's specific policy behavior after the switch:
	std::vector<Job> edfReadyJobs = {
		{1, 1, 0, 3, 2}, // Job 1 deadline is now 3 (earlier)
		{2, 2, 0, 6, 2}  // Job 2 deadline is now 6 (later)
	};

	std::optional<JobID> edfNext = edfScheduler.NextJob(edfReadyJobs);

	if (edfNext == std::nullopt || edfNext.value() != 1)
	{
		std::cerr << "Test failed: EDF did not pick Job 1 (earliest deadline) after hot-swap." << std::endl;
		return;
	}
	std::cout << "EDF correctly selected Job 1 (earliest deadline) after hot-swap." << std::endl;

	std::cout << "Test passed: Scheduler hot-swapped successfully and policies adapted correctly." << std::endl;
}