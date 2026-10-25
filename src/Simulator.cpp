/**
 * @file Simulator.cpp
 * @brief Implements the core real-time simulation engine, managing task releases,
 *        job lifecycles, and scheduler interactions.
 */
#include <Simulator.h>
#include <iostream>

Simulator::Simulator():
	m_scheduler(nullptr),
	m_currentTick(0),
	m_currentJobID(-1),
	m_jobIdCounter(0)
{
}

Simulator::Simulator(Scheduler& _scheduler):
	m_scheduler(&_scheduler),
	m_currentTick(0),
	m_currentJobID(-1),
	m_jobIdCounter(0)
{
}


void Simulator::Reset(const std::vector<TaskConfig>& _taskConfigs)
{
	m_currentTick = 0;
	m_taskConfigs = _taskConfigs;
	m_readyJobs.clear();
	m_allJobsInfo.clear();
	m_simDisplayData.m_executionRecord.clear();
	m_simDisplayData.m_jobDisplay.clear();
	m_currentJobID = -1;
	m_jobIdCounter = 0;
	m_scheduler->PreInitialisation(m_taskConfigs);

	UpdateSimDisplayData();
	//just for initial tick for display purposes
	m_simDisplayData.m_currentTick = 0;
	//clear execution record so that history doesnt update for initial tick
	m_simDisplayData.m_executionRecord.clear();
}

void Simulator::SwitchScheduler(Scheduler& _scheduler)
{
	m_scheduler = &_scheduler;
	m_scheduler->PreInitialisation(m_taskConfigs);
}

void Simulator::AssignTasks()
{
	for (size_t i = 0; i < m_taskConfigs.size(); ++i)
	{
		const TaskConfig& taskConfig = m_taskConfigs[i];
		if (taskConfig.m_period > 0 && m_currentTick % taskConfig.m_period == 0)
		{
			//release new job for this task
			Job newJob;
			newJob.m_id = m_jobIdCounter++;
			newJob.m_taskId = taskConfig.m_id;
			newJob.m_release_time = m_currentTick;
			newJob.m_absoluteDeadline = m_currentTick + taskConfig.m_relativeDeadLine;
			newJob.m_remainingExecution = taskConfig.m_executionTime;
			m_readyJobs.push_back(newJob);
			JobInformation jobInformation
			{
				.m_taskID = newJob.m_taskId,
				.m_jobID = newJob.m_id,
				.m_jobState = JOB_STATE::READY
			};
			m_allJobsInfo.push_back(
				jobInformation
			);
		}
	}
}

void Simulator::AssignJob()
{
	if (m_currentJobID == -1)
	{
		if (m_readyJobs.empty())
		{
			//nothing to run
			std::cout << "unused tick " << m_currentTick << std::endl;
			return;
		}
		else
		{
			std::optional<JobID> nextJobId = m_scheduler->NextJob(m_readyJobs);

			if (nextJobId == std::nullopt)
			{
				//no job returned by scheduler
				std::cerr << "no job returned but there are ready jobs at tick " << m_currentTick + 1 << std::endl;
				return;
			}

			m_currentJobID = GetReadyJob(nextJobId.value()).m_id;

			JobInformation& jobInfo = GetJobInfo(m_currentJobID);

			jobInfo.m_jobState = JOB_STATE::RUNNING;
		}
	}
	else
	{
		std::optional<JobID> newJobID = m_scheduler->EvaluateJobs(m_readyJobs,
			&GetReadyJob(m_currentJobID));
		//if the scheduler requires a job switch, switch to the new job
		if (newJobID != std::nullopt)
		{
			JobInformation& prevJobInfo = GetJobInfo(m_currentJobID);

			prevJobInfo.m_jobState = JOB_STATE::PAUSED;

			std::cout << "Override job " << m_currentJobID << " with new job " << newJobID.value() << " at tick " << m_currentTick + 1 << std::endl;
			m_currentJobID = GetReadyJob(newJobID.value()).m_id;

			JobInformation& nextJobInfo = GetJobInfo(m_currentJobID);

			nextJobInfo.m_jobState = JOB_STATE::RUNNING;
		}
	}
}

void Simulator::UpdateJobState()
{

	if (UpdateCurrentJobProgress())
	{
		std::cout << "Job " << m_currentJobID << " completed at tick " << m_currentTick + 1 << std::endl;

		JobInformation& jobInfo = GetJobInfo(m_currentJobID);

		jobInfo.m_jobState = JOB_STATE::COMPLETED;

		//job completed
		m_readyJobs.erase(std::remove_if(m_readyJobs.begin(), m_readyJobs.end(),
			[this](const Job& job) { return job.m_id == m_currentJobID; }), m_readyJobs.end());

		m_currentJobID = -1;
	}

	for (size_t i = 0; i < m_readyJobs.size(); ++i)
	{
		if (m_currentTick > m_readyJobs[i].m_absoluteDeadline && m_readyJobs[i].m_remainingExecution > 0)
		{
			std::cerr << "Job " << m_readyJobs[i].m_id << " missed its deadline at tick " << m_currentTick + 1 << std::endl;

			//case for when current job missed its deadline
			if (m_currentJobID == m_readyJobs[i].m_id)
			{
				m_currentJobID = -1;
			}

			//set history of job
			JobInformation& jobInfo = GetJobInfo(m_readyJobs[i].m_id);
			jobInfo.m_jobState = JOB_STATE::MISSED;

			//remove job from ready list
			m_readyJobs.erase(m_readyJobs.begin() + i);
			--i; //adjust index after removal
		}
	}
}

void Simulator::Run()
{
	AssignTasks();

	AssignJob();

	UpdateSimDisplayData();

	UpdateJobState();

	//increment tick
	++m_currentTick;
}
 
const SimulatorDisplayData& Simulator::GetDisplayData() const
{
	return m_simDisplayData;
}


void Simulator::UpdateSimDisplayData()
{
	//offset by 1 more so its easier to view
	m_simDisplayData.m_currentTick = m_currentTick + 1;
	
	//reference task configs for more information
	m_simDisplayData.m_taskConfigs = &m_taskConfigs;

	//not neccessarily very performant 
	//but if i want to capture snapshot of the values
	//i have to do it this way to capture the values of the job
	m_simDisplayData.m_jobDisplay = m_allJobsInfo;

	//store current jobid to track current job
	m_simDisplayData.m_currentJobID = m_currentJobID;

	ExecutionRecord record;
	record.m_jobsAtTick = m_allJobsInfo;
	record.m_tick = m_currentTick + 1;
	m_simDisplayData.m_executionRecord.push_back(record);
}

Job& Simulator::GetReadyJob(JobID _jobId)
{
	for (size_t i = 0; i < m_readyJobs.size(); ++i)
	{
		if (m_readyJobs[i].m_id == _jobId)
		{
			return m_readyJobs[i];
		}
	}
	throw std::runtime_error("Job not found");
}

JobInformation& Simulator::GetJobInfo(JobID _jobID)
{
	for (size_t i = 0; i < m_allJobsInfo.size(); ++i)
	{
		if (m_allJobsInfo[i].m_jobID == _jobID)
		{
			return m_allJobsInfo[i];
		}
	}
	throw std::runtime_error("Job not found");
}

bool Simulator::UpdateCurrentJobProgress()
{
	if (m_currentJobID == -1)
		return false;

	Job& currentJobRef = GetReadyJob(m_currentJobID);

	--currentJobRef.m_remainingExecution;

	int maxProgress = 0;
	for (size_t i = 0;i < m_taskConfigs.size();++i)
	{
		if (m_taskConfigs[i].m_id == currentJobRef.m_taskId)
		{
			maxProgress = m_taskConfigs[i].m_executionTime;
		}
	}
	if (maxProgress == 0)
	{
		throw std::runtime_error("No task matching job id found");
	}
	JobInformation& currentJobInfoRef = GetJobInfo(m_currentJobID);

	//current percentage to job completion as a decimal
	currentJobInfoRef.m_jobCompletionPercentage = (maxProgress - currentJobRef.m_remainingExecution) / 
		static_cast<float>(maxProgress);
	//current percentage to job completion as a percentage
	currentJobInfoRef.m_jobCompletionPercentage = (currentJobInfoRef.m_jobCompletionPercentage) * 100;

	if (currentJobRef.m_remainingExecution == 0)
	{
		return true;
	}

	return false;
}