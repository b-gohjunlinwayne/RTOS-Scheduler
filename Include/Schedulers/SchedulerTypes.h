/**
 * @file SchedulerTypes.h
 * @brief Defines core primitive types, type aliases, and enums used across
 *        the real-time scheduling simulator (TaskIDs, JobIDs, Ticks, and Job States).
 */

#pragma once
#ifndef SCHEDULER_TYPES_H
#define SCHEDULER_TYPES_H
#include <cstdint>

/// Unique identifier type for static tasks.
using TaskID = uint32_t;

/// Priority level type for tasks.
using TaskPriority = uint32_t;


/// Discrete unit time type for simulation ticks.
using Tick = uint64_t;

/**
 * @enum JOB_STATE
 * @brief Represents the lifecycle states of an individual execution job.
 */
enum class JOB_STATE
{
    READY,     ///< Job has been released and is waiting in the ready queue.
    RUNNING,   ///< Job is currently executing on the processor.
    PAUSED,    ///< Job execution has been preempted by a higher-priority job.
    COMPLETED, ///< Job has finished its execution time before its deadline.
    MISSED     ///< Job failed to complete before its absolute deadline.
};

/// Unique identifier type for dynamic job instances.
using JobID = uint32_t;

#endif