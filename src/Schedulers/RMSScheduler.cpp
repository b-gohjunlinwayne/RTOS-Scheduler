/**
 * @file RMSScheduler.cpp
 * @brief Implements Rate Monotonic Scheduling (RMS) initialization
 *        and period-based priority job selection.
 */

#include <Schedulers/RMSScheduler.h>
#include <algorithm>
#include <iostream>

void RMSScheduler::PreInitialisation(const std::vector<TaskConfig>& _taskConfigs)
{
	m_taskConfigs = _taskConfigs;
	std::sort(m_taskConfigs.begin(), m_taskConfigs.end(), [](const TaskConfig& a, const TaskConfig& b) {
		return a.m_period < b.m_period;
	});

	Scheduler::PreInitialisation(_taskConfigs);
}

std::optional<JobID> RMSScheduler::NextJob(const std::vector<Job>& _readyJobs) const
{
	if (_readyJobs.empty())
	{
		return std::nullopt;
	}

	//we already ordered the task configs by period in PreInitialisation, so we can just iterate through them and find the first job that matches
	for(const auto& taskConfig : m_taskConfigs)
	{
		//look for first job in the ready list that matches the current task config
		for(const auto& job : _readyJobs)
		{
			if(job.m_taskId == taskConfig.m_id)
			{
				//highest pirority job in the job list found
				return job.m_id;
			}
		}
	}
	throw std::runtime_error("unable to add ready job something went wrong");
}