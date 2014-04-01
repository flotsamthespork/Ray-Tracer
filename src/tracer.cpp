
#include <sys/time.h>
#include <vector>
#include <iostream>
#include "scene.hpp"
#include "job.hpp"

#include "tracer.hpp"

#define AA 3
#define AA_STEP (1.0/AA)

RayTracer::RayTracer(Scene *scene,
		IntersectionStrategy *is) :
	m_scene(scene),
	m_intersect(is),
	m_ambient(0.1),
	m_num_threads(1),
	m_camera(0),
	m_background(0),
	m_first_run(true)
{
	set_light_samples(1);
}

RayTracer::~RayTracer()
{
	delete m_scene;
	delete m_intersect;
}

void
RayTracer::set_background(ColorMap *map)
{
	m_background = map;
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
void
RayTracer::set_light_samples(const int samples)
{
	m_ss_samples = std::max(1,samples);
	m_scene->set_light_samples(m_ss_samples);
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

	std::cout << "Starting" << std::endl;

	if (m_first_run)
	{
		m_scene->fill(m_intersect);
		m_first_run = false;
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

	// TODO - background color
	Colour ray_color(0);
	Colour bg_color(0);
	if (m_background)
	{
		double uv[2];
		uv[0] = (double)x/m_img_width;
		uv[1] = (double)y/m_img_height;
		bg_color = m_background->get_color(uv);
	}

#if !(AA)
	Point3D ray_pos = m_px_to_wcs * Point3D(x+0.5, y+0.5, 0);
	Vector3D ray_dir = ray_pos - m_camera->get_pos();

	Ray r(m_camera->get_pos(), ray_dir, 1, 1, NULL, 0);

	ray(&r, ray_color, data);
#else

	

//#define SAMPLES 2
//	const double d = 1.0/SAMPLES;
//#define AA 0
//#define AA_STEP (1.0/AA)

	Jitter jt(Point3D(x,y,0),
			Vector3D(1,0,0),
			Vector3D(0,1,0));
	jt.set_shape(JITTER_RECTANGLE);
	jt.set_ustep(AA_STEP);
	jt.set_vstep(AA_STEP);

	Colour c[9];

	int i = 0;
	{
//		Point3D ray_pos = m_px_to_wcs * jt.jitter((0.5+0)*AA_STEP, (0.5+0)*AA_STEP);
		Point3D ray_pos = m_px_to_wcs * Point3D(x,y,0);
		Vector3D ray_dir = ray_pos - m_camera->get_pos();
		Ray r(m_camera->get_pos(), ray_dir, 1, 1, NULL, 0);
		if (!ray(&r, c[i++], data))
			c[i-1] = bg_color;
	}
	{
//		Point3D ray_pos = m_px_to_wcs * jt.jitter((0.5+2)*AA_STEP, (0.5+0)*AA_STEP);
		Point3D ray_pos = m_px_to_wcs * Point3D(x+1,y,0);
		Vector3D ray_dir = ray_pos - m_camera->get_pos();
		Ray r(m_camera->get_pos(), ray_dir, 1, 1, NULL, 0);
		if (!ray(&r, c[i++], data))
			c[i-1] = bg_color;
	}
	{
//		Point3D ray_pos = m_px_to_wcs * jt.jitter((0.5+0)*AA_STEP, (0.5+2)*AA_STEP);
		Point3D ray_pos = m_px_to_wcs * Point3D(x,y+1,0);
		Vector3D ray_dir = ray_pos - m_camera->get_pos();
		Ray r(m_camera->get_pos(), ray_dir, 1, 1, NULL, 0);
		if (!ray(&r, c[i++], data))
			c[i-1] = bg_color;
	}
	{
//		Point3D ray_pos = m_px_to_wcs * jt.jitter((0.5+2)*AA_STEP, (0.5+2)*AA_STEP);
		Point3D ray_pos = m_px_to_wcs * Point3D(x+1,y+1,0);
		Vector3D ray_dir = ray_pos - m_camera->get_pos();
		Ray r(m_camera->get_pos(), ray_dir, 1, 1, NULL, 0);
		if (!ray(&r, c[i++], data))
			c[i-1] = bg_color;
	}

	bool sample_more = false;
	for (int j = 0; j < i && !sample_more; ++j)
	{
		for (int k = j+1; k < i && !sample_more; ++k)
		{
			if (c[j].difference(c[k]) > 0.2)
				sample_more = true;
		}
	}

	if (sample_more)
	{
		for (int j = 0; j < 3; ++j)
		{
			Point3D ray_pos = m_px_to_wcs * jt.jitter((0.5+j)*AA_STEP, (0.5+1)*AA_STEP);
			Vector3D ray_dir = ray_pos - m_camera->get_pos();
			Ray r(m_camera->get_pos(), ray_dir, 1, 1, NULL, 0);
			if (!ray(&r, c[i++], data))
				c[i-1] = bg_color;
		}
		for (int j = 0; j < 3; j += 2)
		{
			Point3D ray_pos = m_px_to_wcs * jt.jitter((0.5+1)*AA_STEP, (0.5+j)*AA_STEP);
			Vector3D ray_dir = ray_pos - m_camera->get_pos();
			Ray r(m_camera->get_pos(), ray_dir, 1, 1, NULL, 0);
			if (!ray(&r, c[i++], data))
				c[i-1] = bg_color;
		}
	}

	for (int j = 0; j < i; ++j)
		ray_color = ray_color + (1.0/i)*c[j];

#endif
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
RayTracer::light(const Ray *light_ray,
		IntersectionData &i,
		JobData *data)
{
	const Vector3D view_dir = -1*light_ray->get_dir();
	const Point3D point = light_ray->get_pos() + i.t*light_ray->get_dir();

	Material *mat = i.object->get_material();

	ColorMap *cm_d = mat->get_diffuse();
	ColorMap *cm_s = mat->get_specular();
	ColorMap *cm_b = mat->get_bump();
	const double shiny = mat->get_shininess();
	const double reflect = mat->get_mirror_coefficient();
	const double refract = mat->get_refraction_index();
	const bool is_dielectric = mat->is_dielectric();

	Colour kd = cm_d->get_color(i.uv);
	Colour ks(0);
	if (cm_s)
		ks = cm_s->get_color(i.uv);

	Vector3D normal = i.normal;
	normal.normalize();

	if (cm_b)
	{
		cm_b->bump(normal, i.u_tangent, i.uv);
		normal.normalize();
	}

	Colour diffuse_color = m_ambient * kd;
	Colour specular_color(0);

	const int nlights = m_scene->get_light_count();
	for (int idx = 0; idx < nlights; ++idx)
	{
		double light_contribution;
		int n_samples;
		const Light *l = m_scene->get_light(idx);
		if (l->jitter.get_shape() == JITTER_POINT)
		{
			light_contribution = 1;
			n_samples = 1;
		}
		else
		{
			n_samples = m_ss_samples;
			light_contribution = 1.0/(n_samples*n_samples);
		}

		for (int u = 0; u < n_samples; ++u)
		{
			for (int v = 0; v < n_samples; ++v)
			{
				double ju = ((double)u)/n_samples-0.5;
				double jv = ((double)v)/n_samples-0.5;
				Vector3D light_dir = l->jitter.jitter(ju, jv) - point;
				const double r = light_dir.length();
				light_dir.normalize();

				const double nl = light_dir.dot(normal);

				// Light is behind
				if (nl < 0)
					continue;

				Ray shadow_ray(point, light_dir, 1, 1, NULL, true);
				Intersection light_i(data);
				m_intersect->get_intersection(&shadow_ray, &light_i);

				// Somthing is between light and object
				if (light_i.intersects() &&
					light_i.get_data().t < r)
					continue;
				
				Vector3D light_view_vec = view_dir + light_dir;
				light_view_vec.normalize();

				Colour c = l->color;
				c = (nl /
					(l->falloff[0] +
					 l->falloff[1]*r +
					 l->falloff[2]*r*r)) * c;

				if (cm_s)
				{
					Vector3D r_vec = (2*nl)*normal - light_dir;
					const double rv = std::max(0.0, r_vec.dot(light_view_vec));
					specular_color = specular_color +
						light_contribution*(pow(rv,shiny) / nl)*ks*c;
				}

				diffuse_color = diffuse_color + light_contribution*kd*c;
			}
		}
/*
		Vector3D light_dir = l->jitter.jitter(0,0) - point;
		const double r = light_dir.length();
		light_dir.normalize();

		const double nl = light_dir.dot(normal);

		// Light is behind
		if (nl < 0)
			continue;

		Ray shadow_ray(point, light_dir, 1, 1, NULL, true);
		Intersection light_i(data);
		m_intersect->get_intersection(&shadow_ray, &light_i);

		// Somthing is between light and object
		if (light_i.intersects() &&
			light_i.get_data().t < r)
			continue;
		
		Vector3D light_view_vec = view_dir + light_dir;
		light_view_vec.normalize();

		Colour c = l->color;
		c = (nl /
			(l->falloff[0] +
			 l->falloff[1]*r +
			 l->falloff[2]*r*r)) * c;

		if (cm_s)
		{
			Vector3D r_vec = (2*nl)*normal - light_dir;
			const double rv = std::max(0.0, r_vec.dot(light_view_vec));
			specular_color = specular_color + (pow(rv,shiny) / nl)*ks*c;
		}

		diffuse_color = diffuse_color + kd*c;
		*/
	}

	const double power = light_ray->get_power();

	if (power > 0.01)
	{
		// TODO - refract and get another coefficient for the reflectiveness c:

		// TODO - for refraction - multiply light_color*refract_color

		const double rdn = light_ray->get_dir().dot(normal);

		if (refract && light_ray->get_bounce() < 500)
		{
			double n1, n2;
			n1 = light_ray->get_refraction_index();
			SceneObject *obj;
			if (i.object != light_ray->get_refraction_src())
			{
				n2 = mat->get_refraction_index();
				obj = i.object;
			}
			else
			{
				normal = -normal;
				n2 = 1;
				obj = NULL;
			}
			// TODO - use the fresnel stuff to reflect for dielectric materials
//	https://github.com/TomCrypto/Lambda/blob/master/src/materials/smoothglass.cpp

			const double n = n1 / n2;
			double st2 = 1.0 - n*n*(1.0-rdn*rdn);
			if (st2 >= 0)
			{
				st2 = sqrt(st2);
//				Vector3D ray_dir = (n*light_ray->get_dir()) +
//					(-n*rdn + sqrt(1-st2)) * normal;
				Vector3D ray_dir = n*light_ray->get_dir() +
					(n*std::abs(rdn) - st2)*normal;
				Ray r(point, ray_dir, power, n2, obj, light_ray->get_bounce()+1);
				Colour r_color(0);

				if (ray(&r, r_color, data))
				{
//					diffuse_color = diffuse_color*r_color;
					diffuse_color = r_color;
				}
			}
			else
			{
				Colour r_color(0);
				Vector3D ray_dir = light_ray->get_dir() + 2*rdn*normal;
				Ray r(point, ray_dir, power,
					light_ray->get_refraction_index(),
					light_ray->get_refraction_src(), light_ray->get_bounce()+1);
				if (ray(&r, r_color, data))
					diffuse_color = r_color;
				// reflection?
			}
		}

		if (reflect)
		{
	//		std::cout << reflect << std::endl;
			diffuse_color = (1-reflect)*diffuse_color;

			Colour r_color(0);
			Vector3D ray_dir = light_ray->get_dir() - 2*rdn*normal;
			Ray r(point, ray_dir, power*reflect,
					light_ray->get_refraction_index(),
					light_ray->get_refraction_src(), light_ray->get_bounce());
			if (ray(&r, r_color, data))
				diffuse_color = diffuse_color + reflect*r_color;
		}
	}

	return diffuse_color + specular_color;
}
