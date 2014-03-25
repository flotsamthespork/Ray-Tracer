
#include <sys/time.h>
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

	struct timeval t1, t2;
	gettimeofday(&t1, 0);

	Image img(width, height, 3);
	JobFactory factory(this,
			width*height,
			m_num_threads);
	m_img_width = width;
	m_img_height= height;
	m_px_to_wcs = m_camera->px_to_wcs(width, height);
	m_img = &img;

	factory.run();

	gettimeofday(&t2, 0);
	std::cout << "Time: " <<
		(double)((t2.tv_sec-t1.tv_sec)*1000000 +
				t2.tv_usec-t1.tv_usec)/1000000 << std::endl;

	img.savePng(img_name);

}


void
RayTracer::trace_px(const int px,
		JobData *data)
{
	const int x = px % m_img_width;
	const int y = px / m_img_width;

	if (y == 139 && (x == 250 || x == 267))
		std::cout << "WOOF" << std::endl;

	// TODO - background color
	Colour ray_color(0);

	Point3D ray_pos = m_px_to_wcs * Point3D(x+0.5, y+0.5, 0);
	Vector3D ray_dir = ray_pos - m_camera->get_pos();

	Ray r(m_camera->get_pos(), ray_dir);

	ray(&r, ray_color, data);

	(*m_img)(x,y,0) = ray_color.R();
	(*m_img)(x,y,1) = ray_color.G();
	(*m_img)(x,y,2) = ray_color.B();
}


bool
RayTracer::ray(const Ray *ray, Colour &ray_color,
		JobData *data)
{
	Intersection i(data);
	m_intersect->get_intersection(ray, &i);
	if (i.intersects())
	{
		ray_color = light(ray, i.get_data(), data);
		return true;
	}
	return false;
}


Colour
RayTracer::light(const Ray *ray,
		IntersectionData &i,
		JobData *data)
{
	const Vector3D view_dir = -1*ray->get_dir();
	const Point3D point = ray->get_pos() + i.t*ray->get_dir();

	Material *mat = i.object->get_material();

	ColorMap *cm_d = mat->get_diffuse();
	ColorMap *cm_s = mat->get_specular();
	ColorMap *cm_b = mat->get_bump();

	Colour kd = cm_d->get_color(i.uv);

	Colour light_color = m_ambient * kd;

	// TODO - bump mapping (change that normal broski!)
	Vector3D normal = i.normal;
	normal.normalize();

	if (cm_b)
	{
		cm_b->bump(normal, i.u_tangent, i.uv);
		normal.normalize();
	}

//	const Colour color;
//	const Point3D position;
//	double falloff[3];

	const int nlights = m_scene->get_light_count();
//	std::cout << nlights << std::endl;

	for (int idx = 0; idx < nlights; ++idx)
	{
		const Light *l = m_scene->get_light(idx);
		Vector3D light_dir = l->position - point;
		const double r = light_dir.length();
		light_dir.normalize();

		const double nl = light_dir.dot(normal);

		// Light is behind
		if (nl < 0)
			continue;

		Ray light_ray(point, light_dir, true);
		Intersection light_i(data);
		m_intersect->get_intersection(&light_ray, &light_i);

		// Somthing is between light and object
		if (light_i.intersects() &&
			light_i.get_data().t < r)
			continue;
		
		Vector3D light_view_vec = view_dir + light_dir;
		light_view_vec.normalize();

		Colour c = l->color;
		c = (normal.dot(light_dir) /
			(l->falloff[0] +
			 l->falloff[1]*r +
			 l->falloff[2]*r*r)) * c;

		Vector3D r_vec = (2*nl)*normal - light_dir;
		const double rv = std::max(0.0, r_vec.dot(light_view_vec));
		Colour p = kd;

		light_color = light_color + p*c;
	}

	return light_color;
}
