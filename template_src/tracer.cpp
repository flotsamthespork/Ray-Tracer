
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
	scene->fill(is);
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
	m_ambient = color;
}


void
RayTracer::set_num_threads(const int num_threads)
{
	if (num_threads >= 1)
		m_num_threads = num_threads;
}

IntersectionStrategy*
RayTracer::get_intersection_strategy() const
{
	return m_intersect;
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
	m_img_width = width;
	m_img_height= height;
	m_px_to_wcs = m_camera->px_to_wcs(width, height);
	m_img = &img;

	factory.run();

	img.savePng(img_name);
}


void
RayTracer::trace_px(const int px,
		IntersectionCache *cache)
{
	const int x = px % m_img_width;
	const int y = px / m_img_width;

	// TODO - background color

	Colour ray_color(0);

	Point3D ray_pos = m_px_to_wcs * Point3D(x+0.5, y+0.5, 0);
	Vector3D ray_dir = ray_pos - m_camera->get_pos();

	Ray r(m_camera->get_pos(), ray_dir);

	ray(&r, ray_color, cache);

	(*m_img)(x,y,0) = ray_color.R();
	(*m_img)(x,y,1) = ray_color.G();
	(*m_img)(x,y,2) = ray_color.B();
}


bool
RayTracer::ray(const Ray *ray, Colour &ray_color,
		IntersectionCache *cache)
{
	Intersection i(cache);
	m_intersect->get_intersection(ray, &i);
	if (i.intersects())
	{
		ray_color = Colour(1);
		return true;
	}
	return false;
}
