#pragma once
#include <vector>
#include <Schedulers.h>

class TestScheduler
{
public:
	static void TestEDFScheduler();
	static void TestRMSScheduler();
	static void TestFPPSScheduler();

	static void CompleteJob(std::vector<Job>& _readyJobs, JobID _jobId);

	static const Job& GetJob(JobID _jobId, const std::vector<Job>& _readyJobs);
};