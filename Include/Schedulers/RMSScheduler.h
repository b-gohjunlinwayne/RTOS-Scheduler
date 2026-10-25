/**
 * @file RMSScheduler.h
 * @brief Defines the Rate Monotonic Scheduler (RMS), which prioritizes
 *        jobs based on their task periods (shorter period receives higher priority).
 */
#pragma once
#include <Schedulers/Scheduler.h>

 /**
  * @class RMSScheduler
  * @brief Implements Rate Monotonic Scheduling (RMS) static priority algorithm.
  */
class RMSScheduler : public Scheduler
{
	//store as reference list for task pirority
	std::vector<TaskConfig> m_taskConfigs;

public:
	/**
	 * @brief Pre-initializes the RMS scheduler with task configurations.
	 * @param _taskConfigs Vector of task configurations.
	 */
	void PreInitialisation(
		const std::vector<TaskConfig>& _taskConfigs)
		override;

	/**
	 * @brief Selects the next job to run based on the shortest period (highest rate).
	 * @param _readyJobs Constant reference to the ready job queue.
	 * @return The JobID of the selected job, or std::nullopt if none available.
	 */
	virtual std::optional<JobID> NextJob(
		const std::vector<Job>& _readyJobs) const
		override;
};