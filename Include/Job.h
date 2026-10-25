/**
 * @file Job.h
 * @brief Defines the Job struct representing a concrete, active instance
 *        of a scheduled task during simulation.
 */

#pragma once
#ifndef JOB_H
#define JOB_H

#include <Schedulers/SchedulerTypes.h>

 /**
  * @struct Job
  * @brief Represents an individual execution instance (job) spawned from a task configuration.
  */
struct Job
{
    JobID m_id;                   ///< Unique identifier for this specific job instance.
    TaskID m_taskId;              ///< ID of the parent task configuration.
    Tick m_release_time;          ///< Time tick when the job was released/spawned.
    Tick m_absoluteDeadline;      ///< Absolute time tick by which the job must finish.
    Tick m_remainingExecution;    ///< Remaining execution ticks required to complete.
};

#endif