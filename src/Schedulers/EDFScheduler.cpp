/**
 * @file EDFScheduler.cpp
 * @brief Implements the Earliest Deadline First (EDF) scheduler selection logic.
 */

#include <Schedulers/EDFScheduler.h>

std::optional<JobID> EDFScheduler::NextJob(const std::vector<Job>& _readyJobs) const
{
	if (_readyJobs.empty())
	{
		return std::nullopt;
	}

	const Job* earliestDeadlineJob = &_readyJobs.front();

	for(size_t i = 0; i < _readyJobs.size(); ++i)
	{
		if (_readyJobs[i].m_absoluteDeadline < earliestDeadlineJob->m_absoluteDeadline)
		{
			earliestDeadlineJob = &_readyJobs[i];
		}
	}

	return earliestDeadlineJob->m_id;
}