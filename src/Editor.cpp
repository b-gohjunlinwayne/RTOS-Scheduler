#include <Editor.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>
#include <unordered_map>

/*
    Just some constants so that i can choose colors
*/
const ImVec4 BLUE = ImVec4(0.5f, 0.5f, 1.0f, 1.0f);
const ImVec4 GREEN = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
const ImVec4 RED = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
const ImVec4 YELLOW = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
const ImVec4 ORANGE = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
const ImVec4 GRAY = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
const ImVec4 WHITE = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

Editor::Editor():
	m_currentSchedulerType(SchedulerType::RMS),
	m_activeScheduler(&m_rmsScheduler),
	m_isPlaying(false),
	m_tickAccumulator(0),
	m_tickInterval(0.5f)
{
	m_taskConfigs = {
		{1, 2, 10, 10,0},
		{2, 3, 5, 20,0},
		{3, 2, 15, 15,0}
	};
}

void Editor::Initialise()
{
    m_simulator.SwitchScheduler(*m_activeScheduler);
    m_simulator.Reset(m_taskConfigs);
}

void Editor::Update()
{
    // 1. Get the current main viewport (the active screen area)
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // 2. Set position to the center of the viewport, using a (0.5, 0.5) pivot so it anchors from its own center
    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {

        // 1. Accumulate frame time if playing
        if (m_isPlaying)
        {
            m_tickAccumulator += ImGui::GetIO().DeltaTime;
            if (m_tickAccumulator >= m_tickInterval)
            {
                m_simulator.Run();
                m_tickAccumulator = 0.0f; // Reset accumulator for the next tick
            }
        }

        RenderInformationPanels();
        RenderToolControls();
        RenderTaskConfigTable();
    }

    // Rendering
    ImGui::Render();
}

void Editor::RenderToolControls()
{
    ImGui::Begin("controls");
    {
        const char* schedulerNames[] = { "Rate Monotonic (RMS)", "Earliest Deadline First (EDF)", "Fixed Priority Preemptive (FPPS)" };

        int currentIndex = static_cast<int>(m_currentSchedulerType);

        if (ImGui::Combo("Scheduling Algorithm", &currentIndex, schedulerNames, 3))
        {
            SchedulerType selectedType = static_cast<SchedulerType>(currentIndex);

            if (selectedType != m_currentSchedulerType)
            {
                m_currentSchedulerType = selectedType;

                // Switch the active scheduler pointer based on selection
                switch (m_currentSchedulerType)
                {
                case SchedulerType::RMS:  m_activeScheduler = &m_rmsScheduler; break;
                case SchedulerType::EDF:  m_activeScheduler = &m_edfScheduler; break;
                case SchedulerType::FPPS: m_activeScheduler = &m_fppsScheduler; break;
                default: break;
                }

                m_simulator.SwitchScheduler(*m_activeScheduler);
            }
        }
        ImGui::Separator();

        // --- 1. Play / Pause Button ---
        const char* playPauseButtonText = m_isPlaying ? "Pause" : "Play";
        if (ImGui::Button(playPauseButtonText, ImVec2(80, 0)))
        {
            m_isPlaying = !m_isPlaying;
        }

        ImGui::SameLine();

        // --- 2. Step Tick Button (Only active when paused) ---
        ImGui::BeginDisabled(m_isPlaying);
        {
            if (ImGui::Button("Step Tick", ImVec2(90, 0)))
            {
                m_simulator.Run();
            }
        }
        ImGui::EndDisabled();

        ImGui::SameLine();

        // --- 3. Reset / Stop Button ---
        if (ImGui::Button("Reset", ImVec2(80, 0)))
        {
            m_isPlaying = false;
            m_simulator.Reset(m_taskConfigs);
            m_tickAccumulator = 0.0f;
        }

        ImGui::Separator();

        // --- 4. Speed Control Slider ---
        ImGui::SliderFloat("Tick Speed (s)", &m_tickInterval, 0.05f, 2.0f, "%.2fs");

        // Display current tick status
        ImGui::Text("Current Tick: %zu", m_simulator.GetDisplayData().m_currentTick);
        ImGui::Text("Status: %s", m_isPlaying ? "Running..." : "Paused / Stopped");

    }
    ImGui::End();
}

void Editor::RenderSchedulerTable(const SimulatorDisplayData& _displayData)
{
    ImGui::Begin("Current Task Information");
    {
        if (ImGui::BeginTable("SchedulerTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("Execute Time");
            ImGui::TableSetupColumn("Period");
            ImGui::TableSetupColumn("Relative Deadline");
            ImGui::TableSetupColumn("Pirority");
            ImGui::TableHeadersRow();
            for (const auto& currentTaskData : *_displayData.m_taskConfigs)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", currentTaskData.m_id);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%d", currentTaskData.m_executionTime);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d", currentTaskData.m_period);
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%d", currentTaskData.m_relativeDeadLine);
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%d", currentTaskData.m_priority);
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}

void Editor::RenderTaskConfigTable()
{
    ImGui::Begin("Task Configuration Editor");
    {
        // --- Global Actions ---
        if (ImGui::Button("Add New Task"))
        {
            // Auto-generate a unique ID based on the last task or size
            TaskID newId = m_taskConfigs.empty() ? 1 : m_taskConfigs.back().m_id + 1;

            // Push a default task config (id, exec, period, deadline, priority)
            m_taskConfigs.push_back({ newId, 2, 10, 10, 0});
        }

        ImGui::SameLine();
        // Reset/Apply button because changing task configs requires restarting the engine state
        if (ImGui::Button("Apply Changes & Reset Sim"))
        {
            m_isPlaying = false;
            m_simulator.Reset(m_taskConfigs);
            m_tickAccumulator = 0.0f;
        }

        ImGui::Spacing();

        if (ImGui::BeginTable("SchedulerTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("Execute Time");
            ImGui::TableSetupColumn("Period");
            ImGui::TableSetupColumn("Relative Deadline");
            ImGui::TableSetupColumn("Priority");
            ImGui::TableSetupColumn("Action");
            ImGui::TableHeadersRow();

            // Loop through the editor's mutable task configs directly
            for (size_t i = 0; i < m_taskConfigs.size(); ++i)
            {
                auto& task = m_taskConfigs[i];
                ImGui::TableNextRow();

                // 1. ID (Read-only display)
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", task.m_id);

                // 2. Execution Time
                ImGui::TableSetColumnIndex(1);
                ImGui::PushID(static_cast<int>(i * 10 + 1));
                ImGui::InputScalar("##exec", ImGuiDataType_U64, &task.m_executionTime);
                ImGui::PopID();

                // 3. Period
                ImGui::TableSetColumnIndex(2);
                ImGui::PushID(static_cast<int>(i * 10 + 2));
                ImGui::InputScalar("##period", ImGuiDataType_U64 ,&task.m_period);
                ImGui::PopID();

                // 4. Relative Deadline
                ImGui::TableSetColumnIndex(3);
                ImGui::PushID(static_cast<int>(i * 10 + 3));
                ImGui::InputScalar("##deadline", ImGuiDataType_U64, &task.m_relativeDeadLine);
                ImGui::PopID();

                // 5. Priority
                ImGui::TableSetColumnIndex(4);
                ImGui::PushID(static_cast<int>(i * 10 + 4));
                ImGui::InputScalar("##priority", ImGuiDataType_U32, &task.m_priority);
                ImGui::PopID();

                // 6. Delete Row Action
                ImGui::TableSetColumnIndex(5);
                ImGui::PushID(static_cast<int>(i * 10 + 5));
                if (ImGui::Button("Delete"))
                {
                    m_taskConfigs.erase(m_taskConfigs.begin() + i);
                    ImGui::PopID();
                    break; // Break out to prevent vector iterator invalidation mid-loop
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}

void Editor::RenderTimeline(const SimulatorDisplayData& _displayData)
{
    ImGui::Begin("Execution Timeline (Gantt Chart)");
    {
        if (_displayData.m_taskConfigs == nullptr || _displayData.m_taskConfigs->empty())
        {
            ImGui::Text("No task configuration available.");
            ImGui::End();
            return;
        }

        // Layout sizing constants
        const float rowHeight = 30.0f;
        const float tickWidth = 25.0f;  // Width in pixels per tick column
        const float labelWidth = 100.0f; // Width for task labels on the left

        // Create a scrollable child window for the timeline
        ImGui::BeginChild("TimelineScrollRegion", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 canvasStart = ImGui::GetCursorScreenPos();

            size_t totalTicks = _displayData.m_currentTick + 5; // Add a little padding for future ticks
            if (totalTicks < 20) totalTicks = 20;               // Minimum width window

            // Draw grid header (Tick numbers)
            for (uint64_t t = 1; t <= totalTicks; ++t)
            {
                float x = canvasStart.x + labelWidth + (static_cast<float>(t) * tickWidth);
                float y = canvasStart.y;

                // Draw tick number label
                drawList->AddText(ImVec2(x + 5, y), IM_COL32(150, 150, 150, 255), std::to_string(t).c_str());
                // Draw vertical grid line
                drawList->AddLine(ImVec2(x, y + 20), ImVec2(x, y + 20 + (_displayData.m_taskConfigs->size() * rowHeight)), IM_COL32(60, 60, 60, 100));
            }

            // Draw rows for each task
            float currentY = canvasStart.y + 20.0f;
            for (const auto& task : *_displayData.m_taskConfigs)
            {
                // Draw Task Label on the left
                drawList->AddText(ImVec2(canvasStart.x + 10, currentY + 5), IM_COL32(255, 255, 255, 255), ("Task " + std::to_string(task.m_id)).c_str());

                // Draw background row separator
                drawList->AddLine(
                    ImVec2(canvasStart.x, currentY + rowHeight),
                    ImVec2(canvasStart.x + labelWidth + (totalTicks * tickWidth), currentY + rowHeight),
                    IM_COL32(50, 50, 50, 150)
                );

                currentY += rowHeight;
            }

            //idle row add to end to explicitly state blank
            size_t idleRowIndex = _displayData.m_taskConfigs->size();

            // Render execution history blocks
            for (const auto& record : _displayData.m_executionRecord)
            {
                if (record.m_tick == 0) continue;

                // Group or evaluate job states per task for this tick
                // To handle multiple jobs per task, we find the highest priority state for each task ID at this tick.

                // Map to store the highest priority state found for each task ID at this tick
                std::unordered_map<uint64_t, JOB_STATE> taskHighestState;

                for (const auto& jobInfo : record.m_jobsAtTick)
                {
                    uint64_t taskId = jobInfo.m_taskID;
                    JOB_STATE currentState = jobInfo.m_jobState;

                    // Apply priority: RUNNING > READY > COMPLETED
                    if (taskHighestState.find(taskId) == taskHighestState.end())
                    {
                        taskHighestState[taskId] = currentState;
                    }
                    else
                    {
                        JOB_STATE existingState = taskHighestState[taskId];
                        if (currentState == JOB_STATE::RUNNING) {
                            taskHighestState[taskId] = JOB_STATE::RUNNING; // Running always wins
                        }
                        else if (currentState == JOB_STATE::READY && existingState != JOB_STATE::RUNNING) {
                            taskHighestState[taskId] = JOB_STATE::READY;
                        }
                    }
                }

                // Now render each task row based on its resolved highest-priority state for this tick
                for (const auto& [taskId, jobState] : taskHighestState)
                {
                    auto it = std::find_if(_displayData.m_taskConfigs->begin(), _displayData.m_taskConfigs->end(),
                        [taskId](const TaskConfig& cfg) { return cfg.m_id == taskId; });

                    if (it == _displayData.m_taskConfigs->end()) continue;

                    size_t rowIndex = std::distance(_displayData.m_taskConfigs->begin(), it);
                    ImVec4 blockColor = WHITE;

                    switch (jobState)
                    {
                    case JOB_STATE::RUNNING:   blockColor = GREEN; break;
                    case JOB_STATE::READY:     blockColor = YELLOW; break;
                    case JOB_STATE::COMPLETED: blockColor = BLUE; break;
                    default: continue;
                    }

                    float x1 = canvasStart.x + labelWidth + (static_cast<float>(record.m_tick - 1) * tickWidth);
                    float y1 = canvasStart.y + 20.0f + (static_cast<float>(rowIndex) * rowHeight) + 4.0f;
                    float x2 = x1 + tickWidth - 2.0f;
                    float y2 = y1 + rowHeight - 8.0f;

                    ImU32 u32Color = ImGui::ColorConvertFloat4ToU32(blockColor);
                    drawList->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), u32Color, 3.0f);
                }
            }

            // Reserve layout space so the scrollbar works properly
            ImGui::Dummy(ImVec2(labelWidth + (totalTicks * tickWidth), 20.0f + (_displayData.m_taskConfigs->size() * rowHeight)));
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void Editor::RenderJobStatus(const SimulatorDisplayData& _displayData)
{
    ImGui::Begin("Job Status");
    {
        ImGui::Text("CurrentTick %d", _displayData.m_currentTick);
        if (ImGui::BeginTable("SchedulerTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
        {
            //job id
            ImGui::TableSetupColumn("JobID");
            //whether the job is currently in progress
            ImGui::TableSetupColumn("Status");
            //task progress
            ImGui::TableSetupColumn("Progress");
            //task id
            ImGui::TableSetupColumn("TaskID");
            ImGui::TableHeadersRow();
            for (const auto& currentjobData : _displayData.m_jobDisplay)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", currentjobData.m_jobID);
                ImGui::TableSetColumnIndex(1);
                switch (currentjobData.m_jobState)
                {
                case JOB_STATE::COMPLETED:
                    ImGui::TextColored(BLUE, "COMPLETED");
                    break;
                case JOB_STATE::READY:
                    ImGui::TextColored(YELLOW, "READY");
                    break;
                case JOB_STATE::PAUSED:
                    ImGui::TextColored(YELLOW, "PAUSED");
                    break;
                case JOB_STATE::MISSED:
                    ImGui::TextColored(RED, "MISSED");
                    break;
                case JOB_STATE::RUNNING:
                    ImGui::TextColored(GREEN, "RUNNING");
                    break;
                }
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.1f%%", currentjobData.m_jobCompletionPercentage);
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%d", currentjobData.m_taskID);
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();
}

void Editor::RenderInformationPanels()
{
    const SimulatorDisplayData& displayData = m_simulator.GetDisplayData();

    RenderSchedulerTable(displayData);
    RenderJobStatus(displayData);
    RenderTimeline(displayData);
}