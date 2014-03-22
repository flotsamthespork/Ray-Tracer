
#include "ray.hpp"


//Ray::Ray()


Ray::Ray() :
	m_normalized(false)
{
}


Ray::Ray(const Point3D &ray_pos,
		const Vector3D &ray_dir,
		bool is_normalized) :
	m_pos(ray_pos),
	m_dir(ray_dir),
	m_normalized(is_normalized)

{
	// TODO - possibly not do this.
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

void
Ray::transform(const Matrix4x4 &transform, Ray &ray) const
{
	ray.m_pos = transform * m_pos;
	ray.m_dir = transform * m_dir;
	// TODO - possibly normalize this..
}

