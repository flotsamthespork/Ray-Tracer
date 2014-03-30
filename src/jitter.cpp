
#include <cstdlib>

#include "jitter.hpp"

Jitter::Jitter() :
	m_shape(JITTER_POINT)
{
}

Jitter::Jitter(const Point3D &pos,
		const Vector3D &uvec,
		const Vector3D &vvec) :
	m_shape(JITTER_POINT),
	m_pos(pos),
	m_uvec(uvec),
	m_vvec(vvec)
{
}

double
Jitter::random() const
{
	double r = rand();
	return r / RAND_MAX;
}

void
Jitter::set_shape(JitterShape shape)
{
	m_shape = shape;
}

void
Jitter::set_pos(const Point3D &pos)
{
	m_pos = pos;
}

void
Jitter::set_uvec(const Vector3D &uvec)
{
	m_uvec = uvec;
}

void
Jitter::set_vvec(const Vector3D &vvec)
{
	m_vvec = vvec;
}

void
Jitter::set_ustep(double ustep)
{
	m_ustep = ustep;
}

void
Jitter::set_vstep(double vstep)
{
	m_vstep = vstep;
}

JitterShape
Jitter::get_shape() const
{
	return m_shape;
}

void
Jitter::transform(const Matrix4x4 &transform)
{
	const double uvec_size = m_uvec.length();
	const double vvec_size = m_vvec.length();

	m_pos = transform * m_pos;
	m_uvec = transform * m_uvec;
	m_vvec = transform * m_vvec;

	const double uvec_size_f = m_uvec.length();
	const double vvec_size_f = m_vvec.length();

	m_uvec.normalize();
	m_vvec.normalize();

	m_uvec = uvec_size * m_uvec;
	m_vvec = vvec_size * m_vvec;

	m_ustep *= uvec_size_f/uvec_size;
	m_vstep *= vvec_size_f/vvec_size;
}

Point3D
Jitter::jitter(double u, double v) const
{
	if (m_shape == JITTER_POINT)
		return m_pos;
	else
	{
		Point3D pos = m_pos + (u+random()*m_ustep)*m_uvec;
		return pos + (v+random()*m_vstep)*m_vvec;
	}
	// TODO - possibly circle jitter
}
