
#include <vector>
#include <iostream>
#include "scene.hpp"
#include "job.hpp"

#include "tracer.hpp"


RayTracer::RayTracer(Scene *scene,
		IntersectionStrategy *is) :
	m_scene(scene),
	m_intersect(is),
	m_ambient(0.1),
	m_num_threads(1),
	m_camera(0)
{
	// TODO - fill is with the scene bullshittiness
}

RayTracer::~RayTracer()
{
	delete m_scene;
	delete m_intersect;
}

void
RayTracer::set_ambient(Colour &color)
{
}


void
RayTracer::set_num_threads(const int num_threads)
{
	if (num_threads >= 1)
		m_num_threads = num_threads;
}

void
RayTracer::render(const int cam_id, const std::string img_name,
		const int width, const int height)
{
	m_camera = m_scene->get_camera(cam_id);
	if (!m_camera)
	{
		std::cerr << "Invalid camera id" << std::endl;
		return;
	}
	Image img(width, height, 3);
	JobFactory factory(this,
			width*height,
			m_num_threads);

	factory.run();

	img.savePng(img_name);
}

