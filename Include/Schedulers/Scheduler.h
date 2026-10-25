/**
 * @file  Scheduler.h
 * @brief Defines the abstract base Scheduler class, task data structures,
 *        and the interface for real-time scheduling algorithms.
 */

#pragma once
#include <optional>
#include <vector>
#include <Job.h>
#include <Task.h>

 /**
  * @class Scheduler
  * @brief Abstract base class for all real-time scheduling algorithms (RMS, EDF, FPPS).
  */
class Scheduler
{
public:

	/**
	 * @enum LIU_LAYLAND_RESULT
	 * @brief Results of the Liu & Layland utilization bound schedulability test.
	 */
	enum class LIU_LAYLAND_RESULT
	{
		Schedulable,
		Unschedulable,
		PossiblySchedulable
	};
	/**
	 * @brief Evaluates task set utilization against the Liu & Layland bound.
	 * @param _taskConfigs The vector of task configurations to evaluate.
	 * @return LIU_LAYLAND_RESULT indicating schedulability status.
	 */
	LIU_LAYLAND_RESULT LiuAndLayLandTest(const std::vector<TaskConfig>& _taskConfigs);

	/**
	 * @brief Performs pre-initialization checks and logging for the task set.
	 * @param _taskConfigs The task configurations to initialize with.
	 */
	virtual void PreInitialisation(
		const std::vector<TaskConfig>& _taskConfigs);

	/**
	 * @brief Evaluates jobs every tick to determine if preemption or switching is required.
	 * @param _readyJobs Reference to the current ready job queue.
	 * @param _currentJob Pointer to the currently executing job (if any).
	 * @return Optional JobID if a context switch is required, or std::nullopt otherwise.
	 */
	virtual std::optional<JobID> EvaluateJobs(
		std::vector<Job>& _readyJobs,
		Job* _currentJob) const;

	/**
	 * @brief Pure virtual function to select the next job based on the concrete scheduling algorithm.
	 * @param _readyJobs Constant reference to the ready job queue.
	 * @return The JobID of the selected job, or std::nullopt if none available.
	 */
	virtual std::optional<JobID> NextJob(
		const std::vector<Job>& _readyJobs) const = 0;
};