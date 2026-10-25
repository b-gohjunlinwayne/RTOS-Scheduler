/**
 * @file FPPSScheduler.h
 * @brief Defines the Fixed Priority Preemptive Scheduler (FPPS),
 *        which prioritizes jobs based on assigned task priorities.
 */
#pragma once
#include <Schedulers/Scheduler.h>
#include <optional>

 /**
  * @class FPPSScheduler
  * @brief Implements Fixed Priority Preemptive Scheduling.
  */
class FPPSScheduler : public Scheduler
{
	std::vector<TaskConfig> m_taskConfigs;
public:
	/**
	 * @brief Pre-initializes the FPPS scheduler with task configurations
	 *        and caches them for priority lookups.
	 * @param _taskConfigs Vector of task configurations defining priorities.
	 */
	virtual void PreInitialisation(
		const std::vector<TaskConfig>& _taskConfigs) override;

	/**
	 * @brief Evaluates whether a higher-priority ready job should preempt
	 *        the currently executing job.
	 * @param _readyJobs Reference to the ready job queue.
	 * @param _currentJob Pointer to the currently running job.
	 * @return Optional JobID if preemption should occur, or std::nullopt otherwise.
	 */
	std::optional<JobID> EvaluateJobs(
		std::vector<Job>& _readyJobs,
		Job* _currentJob) const override;

	/**
	 * @brief Selects the next job to run based on the highest assigned static priority.
	 * @param _readyJobs Constant reference to the ready job queue.
	 * @return The JobID of the highest-priority job, or std::nullopt if none available.
	 */
	virtual std::optional<JobID> NextJob(
		const std::vector<Job>& _readyJobs) const override;
};