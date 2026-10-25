/**
 * @file FPPSScheduler.cpp
 * @brief Implements Fixed Priority Preemptive Scheduling (FPPS) initialization,
 *        priority-based preemption evaluation, and job selection.
 */

#include <Schedulers/FPPSScheduler.h>
#include <algorithm>
#include <stdexcept>

void FPPSScheduler::PreInitialisation(
	const std::vector<TaskConfig>& _taskConfigs)
{
	// Default implementation does nothing
	m_taskConfigs = _taskConfigs;

	//sort by higher pirority first, so we can just iterate through them and find the first job that matches
	std::stable_sort(m_taskConfigs.begin(), m_taskConfigs.end(), [](const TaskConfig& a, const TaskConfig& b) {
		return a.m_priority < b.m_priority;
		});

	Scheduler::PreInitialisation(_taskConfigs);
}

std::optional<JobID> FPPSScheduler::EvaluateJobs(
	std::vector<Job>& _readyJobs,
	Job* _currentJob) const
{
	if (_currentJob == nullptr)
	{
		throw std::exception("current job is null something went wrong");
	}
	if (_readyJobs.empty())
	{
		return std::nullopt;
	}
	else
	{
		for (const auto& taskConfig : m_taskConfigs)
		{
			if (taskConfig.m_id == _currentJob->m_taskId)
			{
				//current job highest pirority, no need to switch
				return std::nullopt;
			}
			else 
			{
				for (auto& job : _readyJobs)
				{
					if (job.m_taskId == taskConfig.m_id)
					{
						//found a higher pirority job, switch to it
						return job.m_id;
					}
				}
			}
		}
	}
	throw std::runtime_error("unable to evaluate something went wrong");
}

std::optional<JobID> FPPSScheduler::NextJob(
	const std::vector<Job>& _readyJobs) const
{
	if (_readyJobs.empty())
	{
		return std::nullopt;
	}
	else
	{
		for (const auto& taskConfig : m_taskConfigs)
		{
			for (auto& job : _readyJobs)
			{
				if (job.m_taskId == taskConfig.m_id)
				{
					//found the highest pirority ready job
					return job.m_id;
				}
			}
		}
	}
	throw std::runtime_error("unable to add ready job something went wrong");
}