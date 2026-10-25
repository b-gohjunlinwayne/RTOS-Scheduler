/**
 * @file	Scheduler.cpp
 * @brief	Implements the base Scheduler interface and real-time schedulability analysis,
 *			including the Liu & Layland utilization bound test.
 */
#include <Schedulers/Scheduler.h>
#include <iostream>
#include <cmath>

void Scheduler::PreInitialisation(const std::vector<TaskConfig>& _taskConfigs)
{
	std::cout << "Running Liu and Layland Test..." << std::endl;
	switch (LiuAndLayLandTest(_taskConfigs))
	{
	case LIU_LAYLAND_RESULT::Schedulable:
		std::cout << "Task set is schedulable" << std::endl;
		break;
	case LIU_LAYLAND_RESULT::Unschedulable:
		std::cerr << "Task set is not schedulable" << std::endl;
		break;
	case LIU_LAYLAND_RESULT::PossiblySchedulable:
		std::cerr << "Task set may not be schedulable" << std::endl;
		break;
	}
}

Scheduler::LIU_LAYLAND_RESULT Scheduler::LiuAndLayLandTest(const std::vector<TaskConfig>& _taskConfigs)
{
	//basic liu and layland schedulability test
	unsigned int n = _taskConfigs.size();
	double utilization_bound = n * (pow(2.0, 1.0 / n) - 1);

	//calculate total utilization of the task set
	float utilization = 0.f;
	for (int i = 0; i < _taskConfigs.size(); ++i)
	{
		utilization += (_taskConfigs[i].m_executionTime / (float)_taskConfigs[i].m_period);
	}

	if (utilization > 1.0f)
	{
		return LIU_LAYLAND_RESULT::Unschedulable;
	}
	else if (utilization > utilization_bound)
	{
		return LIU_LAYLAND_RESULT::PossiblySchedulable;
	}

	return LIU_LAYLAND_RESULT::Schedulable;
}

std::optional<JobID> Scheduler::EvaluateJobs(
	std::vector<Job>& _readyJobs,
	Job* _currentJob) const {
	return std::nullopt;
}