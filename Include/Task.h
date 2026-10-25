/**
 * @file Task.h
 * @brief Defines the TaskConfig struct representing the static parameters
 *        of a real-time periodic task.
 */

#pragma once
#include <Schedulers/SchedulerTypes.h>

 /**
  * @struct TaskConfig
  * @brief Holds static configuration parameters required to instantiate real-time tasks.
  */
struct TaskConfig
{
    TaskID m_id;               ///< Unique identifier for the task.
    Tick m_executionTime;      ///< Worst-case execution time (WCET) required per period.
    Tick m_period;             ///< Interval at which task instances (jobs) are released.
    Tick m_relativeDeadLine;   ///< Deadline relative to the job's release time.

    /// Lower number indicates higher priority (0 is the highest priority).
    TaskPriority m_priority;
};