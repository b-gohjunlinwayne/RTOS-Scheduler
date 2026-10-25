/**
 * @file EDFScheduler.h
 * @brief Defines the Earliest Deadline First (EDF) scheduler implementation,
 *        which prioritizes jobs based on their absolute deadlines.
 */

#pragma once
#include <Schedulers/Scheduler.h>
#include <optional>

 /**
  * @class EDFScheduler
  * @brief Implements the Earliest Deadline First dynamic scheduling algorithm.
  */
class EDFScheduler : public Scheduler
{
public:
    /**
     * @brief Selects the next job to execute based on the earliest absolute deadline.
     * @param _readyJobs A vector of currently ready jobs.
     * @return The JobID of the selected job, or std::nullopt if the ready queue is empty.
     */
	std::optional<JobID> NextJob(const std::vector<Job>& _readyJobs) const override;
};