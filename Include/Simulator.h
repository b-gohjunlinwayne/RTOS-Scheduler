/**
 * @file Simulator.h
 * @brief Defines Simulator class and its display information for running
 *		  a scheduler 
 */
#pragma once
#include <cstdint>
#include <Schedulers/Scheduler.h>

/**
 * @struct JobInformation
 * @brief struct containing job information to display
 */
struct JobInformation
{
	TaskID m_taskID;
	JobID m_jobID;
	JOB_STATE m_jobState;
	float m_jobCompletionPercentage;
};

/**
 * @struct SimulatorDisplayData
 * @brief struct containing execution record data
 */
struct ExecutionRecord
{
	Tick m_tick{ 0 };
	// Captures the state of every active/ready job across all tasks at this tick
	std::vector<JobInformation> m_jobsAtTick;
};

/**
 * @struct SimulatorDisplayData
 * @brief struct containing simulator display data
 */
struct SimulatorDisplayData
{
	uint64_t m_currentTick = 0;
	const std::vector<TaskConfig>* m_taskConfigs = nullptr;
	std::vector<JobInformation> m_jobDisplay{};
	JobID m_currentJobID = static_cast<JobID>(-1);
	std::vector<ExecutionRecord> m_executionRecord;
};

/**
 * @class Simulator
 * @brief Implements the simulator to run various scheduling algorithms
 */
class Simulator
{
	uint64_t m_currentTick;

	Scheduler* m_scheduler;

	std::vector<TaskConfig> m_taskConfigs;

	JobID m_currentJobID;

	std::vector<Job> m_readyJobs;

	std::vector<JobInformation> m_allJobsInfo;

	uint32_t m_jobIdCounter;

	SimulatorDisplayData m_simDisplayData;

public:

	/**
	 * @brief Constructs a Simulator instance without scheduling algorithm
	 */
	Simulator();

	/**
	 * @brief Constructs a Simulator instance configured with a specific scheduling algorithm.
	 * @param _scheduler Reference to the concrete scheduler implementation (e.g., RMS, EDF, FPPS).
	 */
	Simulator(Scheduler& _scheduler);

	/**
	 * @brief Resets the simulation state and re-initializes the scheduler with a new task set.
	 * @param _taskConfigs A vector defining the task configurations (periods, execution times, deadlines).
	 */
	void Reset(const std::vector<TaskConfig>& _taskConfigs);

	/**
	 * @brief switches the current scheduler
	 * @param _scheduler current scheduler to use
	 */
	void SwitchScheduler(Scheduler& _scheduler);

	/**
	 * @brief Advances the simulation forward by a single discrete time tick.
	 */
	void Run();

	/**
	 * @brief Retrieves a read-only snapshot of the current simulation state for the UI frontend.
	 * @return Constant reference to SimulatorDisplayData containing tick counts, task lists, and job statuses.
	 */
	const SimulatorDisplayData& GetDisplayData() const;

private:

	/**
	 * @brief Evaluates task periods and releases new jobs into the system when a period boundary is reached.
	 */
	void AssignTasks();

	/**
	 * @brief Queries the active scheduler to select the highest-priority ready job for the current tick.
	 */
	void AssignJob();

	/**
	 * @brief Updates the execution state (Running, Completed, Missed, Ready) of active jobs.
	 */
	void UpdateJobState();

	/**
	 * @brief Captures a thread-safe snapshot of the current simulation state for the ImGui frontend.
	 */
	void UpdateSimDisplayData();

	/*
		Helper functions
	*/

	Job& GetReadyJob(JobID _jobID);

	JobInformation& GetJobInfo(JobID _jobID);

	/**
	 * @brief Update the current job progress
	 * @return true if the current job is finished false otherwise
	 */
	bool UpdateCurrentJobProgress();
};