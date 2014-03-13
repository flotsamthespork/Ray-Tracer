#ifndef __JOB_H__
#define __JOB_H__

#include "tracer.hpp"
#include <vector>
#include <pthread.h>

class JobFactory;

// Specifies a specific job
class RayTracerJob {
private:
	JobFactory *const m_factory;
	const RayTracer *const m_tracer;
	bool m_threaded;
	pthread_t m_thread;
public:
	RayTracerJob(JobFactory *factory,
			const RayTracer *rt);
	~RayTracerJob();

	void run_threaded();
	void run();
	void wait_for_finish();

private:
	static void *_run(void *arg);
};


// JobFactory is responsible for splitting a ray tracing
// job up for a bunch of RayTracerJobs to complete. It is
// pure virtual so we can easily swap in different factories
// and do different jobs.
class JobFactory {
private:
	pthread_mutex_t m_px_lock;
	const int m_last_px;
	volatile int m_next_px;
	std::vector<RayTracerJob*> m_jobs;
public:
	JobFactory(RayTracer *rt,
			const int num_jobs);
	~JobFactory();

	void run();
private:
	bool get_px(int &px_a, int &px_b);

	friend class RayTracerJob;
};



#endif
