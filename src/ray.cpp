
#include "ray.hpp"


//Ray::Ray()


Ray::Ray() :
	m_normalized(false)
{
}


Ray::Ray(const Point3D &ray_pos,
		const Vector3D &ray_dir,
		const double power,
		const double refraction_index,
		SceneObject *refraction_obj,
		const int bounce,
		bool is_normalized) :
	m_pos(ray_pos),
	m_dir(ray_dir),
	m_normalized(is_normalized),
	m_power(power),
	m_refraction_index(refraction_index),
	m_refraction_obj(refraction_obj),
	m_bounce(bounce)

{
	normalize();
}

void
Ray::normalize()
{
	if (m_normalized)
		return;
	m_normalized = true;
	m_dir.normalize();
}

const Point3D&
Ray::get_pos() const
{
	return m_pos;
}

const Vector3D&
Ray::get_dir() const
{
	return m_dir;
}

double
Ray::get_power() const
{
	return m_power;
}

double
Ray::get_refraction_index() const
{
	return m_refraction_index;
}

SceneObject*
Ray::get_refraction_src() const
{
	return m_refraction_obj;
}

int
Ray::get_bounce() const
{
	return m_bounce;
}

void
Ray::transform(const Matrix4x4 *transform, Ray &ray) const
{
	ray.m_pos = *transform * m_pos;
	ray.m_dir = *transform * m_dir;
	ray.m_normalized = false;
	// TODO - possibly normalize this..
}

