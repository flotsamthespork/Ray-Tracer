
#include <iostream>
#include <cstdio>
#include <cassert>
#include <unistd.h>

#include "job.hpp"


RayTracerJob::RayTracerJob(JobFactory *factory, RayTracer *rt) :
	m_factory(factory),
	m_tracer(rt),
	m_threaded(false)
{
	m_data.cache = rt->get_intersection_strategy()->create_intersection_cache();
}


RayTracerJob::~RayTracerJob()
{
	if (m_data.cache)
		delete m_data.cache;
}

static void
set_core(int core, pthread_attr_t *attr)
{
	int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
	core = core % num_cores;

	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core, &cpuset);

	pthread_attr_setaffinity_np(attr, sizeof(cpuset), &cpuset);
}

void
RayTracerJob::run_threaded(int i)
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	m_threaded = true;
	set_core(i, &attr);
	pthread_create(&m_thread, &attr,
			&RayTracerJob::_run, this);
}

void
RayTracerJob::run()
{
	int px, px_f, x, y;
	while (m_factory->get_px(px, px_f))
	{
		for (; px < px_f; ++px)
		{
			m_tracer->trace_px(px, &m_data);
		}
	}
}

void*
RayTracerJob::_run(void *arg)
{
	((RayTracerJob*)arg)->run();
	return 0;
}

void
RayTracerJob::wait_for_finish()
{
	if (m_threaded)
	{
		pthread_join(m_thread, NULL);
	}
}


JobFactory::JobFactory(RayTracer *rt,
		const int num_px,
		const int num_jobs) :
	m_last_px(num_px),
	m_next_px(0)
{
	assert(num_jobs > 0);
	pthread_mutex_init(&m_px_lock, NULL);

	for (int i = 0; i < num_jobs; ++i)
		m_jobs.push_back(new RayTracerJob(this, rt));
}

JobFactory::~JobFactory()
{
	pthread_mutex_destroy(&m_px_lock);
	for (std::vector<RayTracerJob*>::iterator i = m_jobs.begin();
			i != m_jobs.end(); ++i)
		delete (*i);
}


void
JobFactory::run()
{
	if (m_jobs.size() == 1)
	{
		m_jobs[0]->run();
	}
	else
	{
		const int n = m_jobs.size();
		for (int i = 0; i < n; ++i)
			m_jobs[i]->run_threaded(i);
		for (int i = 0; i < n; ++i)
			m_jobs[i]->wait_for_finish();
	}
}

bool
JobFactory::get_px(int &px_a, int &px_b)
{
	int count;
	pthread_mutex_lock(&m_px_lock);
	px_a = m_next_px;
	if (px_a < m_last_px)
	{
		count = std::min(m_last_px-px_a, 25);
		px_b = px_a+count;
		m_next_px += count;
	}
	else
		count = 0;
	pthread_mutex_unlock(&m_px_lock);
	return (count > 0);
}
