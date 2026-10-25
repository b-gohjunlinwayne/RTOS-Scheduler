#pragma once
#include <Schedulers.h>
#include <Simulator.h>

/**
 * @class Editor
 * @brief Manages the Dear ImGui user interface, simulation runtime controls,
 *        and visual rendering panels (timeline, scheduler tables, task configurations)
 *        for the C++20 RTOS Scheduler Simulator.
 */
class Editor
{
	/**
	 * @enum SchedulerType
	 * @brief Supported real-time scheduling algorithms available in the simulator.
	 */
	enum class SchedulerType
	{
		RMS = 0,	///< Rate Monotonic Scheduling (Static priority based on period)
		EDF,		///< Earliest Deadline First (Dynamic priority based on absolute deadlines)
		FPPS,		///< Fixed Priority Preemptive Scheduling
		Count		///< Total number of supported schedulers (used for UI loops/bounds)
	};

	SchedulerType m_currentSchedulerType;


	// Concrete scheduler instances
	RMSScheduler m_rmsScheduler;
	EDFScheduler m_edfScheduler;
	FPPSScheduler m_fppsScheduler;

	// Pointer to whichever scheduler is currently active
	Scheduler* m_activeScheduler;
	Simulator  m_simulator;

	bool m_isPlaying;
	float m_tickAccumulator;
	float m_tickInterval;

	//vector task config
	std::vector<TaskConfig> m_taskConfigs;

public:
	/**
	 * @brief Construct a new Editor object
	 */
	Editor();
	/**
	 * @brief Initialize default states
	 */
	void Initialise();
	/**
	 * @brief Main per-frame update loop handling UI panels and simulation stepping.
	 */
	void Update();

private:
	/**
	 * @brief Renders the top toolbar controls (Play, Pause, Step Tick, Reset).
	 */
	void RenderToolControls();
	/**
	 * @brief Renders general simulation telemetry and status info panels.
	 */
	void RenderInformationPanels();
	/**
	 * @brief Renders the task configuration editor table (periods, execution times, deadlines).
	 */
	void RenderTaskConfigTable();
	/**
	 * @brief Renders the multi-tick execution Gantt chart timeline using ImDrawList.
	 * @param _displayData Snapshot of current simulation ticks, task configurations, and history records.
	 */
	void RenderTimeline(const SimulatorDisplayData& _displayData);
	/**
	 * @brief Renders the active scheduler queue table and priority breakdown.
	 * @param _displayData Snapshot of current simulation data.
	 */
	void RenderSchedulerTable(const SimulatorDisplayData& _displayData);
	/**
	 * @brief Renders current job status indicators and metrics for active tasks.
	 * @param _displayData Snapshot of current simulation data.
	 */
	void RenderJobStatus(const SimulatorDisplayData& _displayData);

};