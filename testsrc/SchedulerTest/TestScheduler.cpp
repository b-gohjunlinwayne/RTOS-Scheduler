/**
 * @file	TestScheduler.cpp
 * @brief	Implements unit testing and validation routines to evaluate 
 *			the behavior of the various real-time scheduler implementations.
 */

#include <TestScheduler.h>
#include <Schedulers.h>
#include <assert.h>
#include <iostream>

void TestScheduler::CompleteJob(std::vector<Job>& _readtJobs,JobID _jobID)
{
	for(int i =0 ; i < _readtJobs.size(); ++i)
	{
		if(_readtJobs[i].m_id == _jobID)
		{
			_readtJobs.erase(_readtJobs.begin() + i);
			return;
		}
	}
}

const Job& TestScheduler::GetJob(JobID _jobId, const std::vector<Job>& _readyJobs)
{
	for(int i = 0; i < _readyJobs.size(); ++i)
	{
		if(_readyJobs[i].m_id == _jobId)
		{
			return _readyJobs[i];
		}
	}
	throw std::runtime_error("Job not found");
}

void TestScheduler::TestEDFScheduler()
{
	std::cout << "Running EDF Scheduler Test..." << std::endl;

	/*
	Task 1: ID=1, ExecutionTime=5, Period=10, RelativeDeadline=10
	Task 2: ID=2, ExecutionTime=3, Period=5, RelativeDeadline=5
	Task 3: ID=3, ExecutionTime=7, Period=15, RelativeDeadline=15
*/
	std::vector<TaskConfig> taskConfigs = {
		{1, 5, 10, 10},
		{2, 3, 5, 5},
		{3, 7, 15, 15}
	};

	/*
		Job 1:
		ID=1, TaskID=1, ReleaseTime=0, AbsoluteDeadline=10, RemainingExecution=5
		Job 2:	
		ID=2, TaskID=2, ReleaseTime=0, AbsoluteDeadline=5, RemainingExecution=3
		Job 3:
		ID=3, TaskID=3, ReleaseTime=0, AbsoluteDeadline=15, RemainingExecution=7
	*/

	//so right order should be 2,1,3 based on earliest deadline first
	std::vector<Job> readyJobs = {
		{1, 1, 0, 10, 5},
		{2, 2, 0, 5, 3},
		{3, 3, 0, 15, 7}
	};

	EDFScheduler scheduler;

	scheduler.PreInitialisation(taskConfigs);

	JobID correctOrder[] = { 2, 1, 3 };

	int currentOrderIndex = 0;

	while (readyJobs.size() > 0)
	{
		std::optional<JobID> nextJobId = scheduler.NextJob(readyJobs);

		if(nextJobId == std::nullopt)
		{
			std::cerr << "Test failed: No job returned by scheduler." << std::endl;
			return;
		}

		const Job& nextJob = GetJob(nextJobId.value(), readyJobs);

		std::cout << "Job ID: " << nextJob.m_id
			<< ", Deadline: " << nextJob.m_absoluteDeadline << std::endl;

		if(nextJob.m_id != correctOrder[currentOrderIndex++])
		{
			std::cerr << "Test failed: Job order is incorrect." << std::endl;
			return;
		}

		CompleteJob(readyJobs, nextJob.m_id);
	}
	std::cout << "Test passed: All jobs scheduled in correct order." << std::endl;
}

void TestScheduler::TestRMSScheduler()
{
	std::cout << "Running RMS Scheduler Test..." << std::endl;
	/*
		Task 1: ID=1, ExecutionTime=5, Period=10, RelativeDeadline=10 Pirority = 0
		Task 2: ID=2, ExecutionTime=3, Period=5, RelativeDeadline=5 Pirority = 0
		Task 3: ID=3, ExecutionTime=7, Period=15, RelativeDeadline=15 Pirority = 0
	*/
	std::vector<TaskConfig> taskConfigs = {
		{1, 5, 10, 10, 0},
		{2, 3, 5, 5 , 0},
		{3, 7, 15, 15 , 0}
	};

	/*
		Based on above task this is the generated jobs at time 0:
		Job 1:
		ID=1, TaskID=1, ReleaseTime=0, AbsoluteDeadline=10, RemainingExecution=5
		Job 2:
		ID=2, TaskID=2, ReleaseTime=0, AbsoluteDeadline=5, RemainingExecution=3
		Job 3:
		ID=3, TaskID=3, ReleaseTime=0, AbsoluteDeadline=15, RemainingExecution=7
	*/

	//so right order should be 2,1,3 based on shortest period first
	std::vector<Job> readyJobs = {
		{1, 1, 0, 10, 5},
		{2, 2, 0, 5, 3},
		{3, 3, 0, 15, 7}
	};

	RMSScheduler scheduler;

	scheduler.PreInitialisation(taskConfigs);
	JobID correctOrder[] = { 2, 1, 3 };

	int currentOrderIndex = 0;

	while (readyJobs.size() > 0)
	{
		std::optional<JobID> nextJobId = scheduler.NextJob(readyJobs);

		if (nextJobId == std::nullopt)
		{
			std::cerr << "Test failed: No job returned by scheduler." << std::endl;
			return;
		}

		const Job& nextJob = GetJob(nextJobId.value(), readyJobs);

		std::cout << "Job ID: " << nextJob.m_id
			<< ", Deadline: " << nextJob.m_absoluteDeadline << std::endl;

		if (nextJob.m_id != correctOrder[currentOrderIndex++])
		{
			std::cerr << "Test failed: Job order is incorrect." << std::endl;
			return;
		}

		CompleteJob(readyJobs, nextJob.m_id);
	}
	std::cout << "Test passed: All jobs scheduled in correct order." << std::endl;

}

void TestScheduler::TestFPPSScheduler()
{
	std::cout << "Running FPPS Scheduler Test..." << std::endl;

	/*
		Task 1: ID=1, ExecutionTime=5, Period=15, RelativeDeadline=15, Priority = 2 (Lowest priority)
		Task 2: ID=2, ExecutionTime=2, Period=10, RelativeDeadline=10, Priority = 1 (Medium priority)
		Task 3: ID=3, ExecutionTime=1, Period=5, RelativeDeadline=5, Priority = 0 (Highest priority)
	*/
	std::vector<TaskConfig> taskConfigs = {
		{1, 5, 15, 15, 2},
		{2, 2, 10, 10, 1},
		{3, 1, 5, 5, 0}
	};

	/*
		Based on above tasks, the generated jobs at time 0:
		Job 1:
		ID=1, TaskID=1, ReleaseTime=0, AbsoluteDeadline=15, RemainingExecution=5
		Job 2:
		ID=2, TaskID=2, ReleaseTime=0, AbsoluteDeadline=10, RemainingExecution=2
		Job 3:
		ID=3, TaskID=3, ReleaseTime=0, AbsoluteDeadline=5, RemainingExecution=1
	*/

	// The right ready queue order should be 3, 2, 1 based on highest priority first (Priority 0 -> 1 -> 2)
	std::vector<Job> readyJobs = {
		{3, 3, 0, 5, 1},
		{2, 2, 0, 10, 2},
		{1, 1, 0, 15, 5}
	};

	FPPSScheduler scheduler;

	scheduler.PreInitialisation(taskConfigs);
	JobID correctOrder[] = { 3, 2, 1 };

	int currentOrderIndex = 0;

	while (readyJobs.size() > 0)
	{
		std::optional<JobID> nextJobId = scheduler.NextJob(readyJobs);

		if (nextJobId == std::nullopt)
		{
			std::cerr << "Test failed: No job returned by scheduler." << std::endl;
			return;
		}

		const Job& nextJob = GetJob(nextJobId.value(), readyJobs);

		std::cout << "Job ID: " << nextJob.m_id
			<< ", Deadline: " << nextJob.m_absoluteDeadline << std::endl;

		if (nextJob.m_id != correctOrder[currentOrderIndex++])
		{
			std::cerr << "Test failed: Job order is incorrect." << std::endl;
			return;
		}

		CompleteJob(readyJobs, nextJob.m_id);
	}
	std::cout << "Test passed: All jobs scheduled in correct order." << std::endl;
}