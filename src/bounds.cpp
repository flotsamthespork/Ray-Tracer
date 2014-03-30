
#include "bounds.hpp"


Bounds::Bounds() :
	m_type(UNSET)
{
}

Bounds::Bounds(const Bounds &b)
{
	copy(b);
}


Bounds::~Bounds()
{
}

void
Bounds::copy(const Bounds &b)
{
	m_type		= b.m_type;
	m_box_pmin	= b.m_box_pmin;
	m_box_pmax	= b.m_box_pmax;
	m_sp_origin	= b.m_sp_origin;
	m_sp_radius	= b.m_sp_radius;
}

double
Bounds::get_left()
{
	if (m_type == BOX)	return m_box_pmin[0];
	else			return m_sp_origin[0]-m_sp_radius;
}

double
Bounds::get_right()
{
	if (m_type == BOX)	return m_box_pmax[0];
	else			return m_sp_origin[0]+m_sp_radius;
}

double
Bounds::get_top()
{
	if (m_type == BOX)	return m_box_pmax[1];
	else			return m_sp_origin[1]+m_sp_radius;
}

double
Bounds::get_bottom()
{
	if (m_type == BOX)	return m_box_pmin[1];
	else			return m_sp_origin[1]-m_sp_radius;
}

double
Bounds::get_front()
{
	if (m_type == BOX)	return m_box_pmin[2];
	else			return m_sp_origin[2]-m_sp_radius;
}

double
Bounds::get_back()
{
	if (m_type == BOX)	return m_box_pmax[2];
	else			return m_sp_origin[2]+m_sp_radius;
}

void
Bounds::set_box(const Point3D &pmin,
		const Point3D &pmax)
{
	m_type = BOX;
	m_box_pmin = pmin;
	m_box_pmax = pmax;
}

void
Bounds::set_sphere(const Point3D &origin,
		const double radius)
{
	m_type = SPHERE;
	m_sp_origin = origin;
	m_sp_radius = radius;
}

void
Bounds::transform(const Matrix4x4 &transform)
{
	if (m_type == BOX)
	{
		Point3D p000 = m_box_pmin;
		Point3D p100(m_box_pmax[0],
				m_box_pmin[1],
				m_box_pmin[2]);
		Point3D p010(m_box_pmin[0],
				m_box_pmax[1],
				m_box_pmin[2]);
		Point3D p001(m_box_pmin[0],
				m_box_pmin[1],
				m_box_pmax[2]);
		Point3D p110(m_box_pmax[0],
				m_box_pmax[1],
				m_box_pmin[2]);
		Point3D p101(m_box_pmax[0],
				m_box_pmin[1],
				m_box_pmax[2]);
		Point3D p011(m_box_pmin[0],
				m_box_pmax[1],
				m_box_pmax[2]);
		Point3D p111 = m_box_pmax;

		p000 = transform * p000;
		p100 = transform * p100;
		p010 = transform * p010;
		p001 = transform * p001;
		p110 = transform * p110;
		p101 = transform * p101;
		p011 = transform * p011;
		p111 = transform * p111;

		bool first = true;
		eval_min_max(p000, m_box_pmin, m_box_pmax, &first);
		eval_min_max(p100, m_box_pmin, m_box_pmax, &first);
		eval_min_max(p010, m_box_pmin, m_box_pmax, &first);
		eval_min_max(p001, m_box_pmin, m_box_pmax, &first);
		eval_min_max(p110, m_box_pmin, m_box_pmax, &first);
		eval_min_max(p101, m_box_pmin, m_box_pmax, &first);
		eval_min_max(p011, m_box_pmin, m_box_pmax, &first);
		eval_min_max(p111, m_box_pmin, m_box_pmax, &first);
	}
	else if (m_type == SPHERE)
	{
		Vector3D v(m_sp_radius,
				m_sp_radius,
				m_sp_radius);
		v = transform * v;

		m_sp_origin = transform * m_sp_origin;
		m_sp_radius = v.length();
	}
}

void
Bounds::merge(const Bounds &b)
{
	if (b.m_type == UNSET)
		return;

	if (m_type == UNSET)
	{
		copy(b);
	}
	else
	{
		if (m_type == SPHERE)
		{
			Vector3D r(m_sp_radius,
					m_sp_radius,
					m_sp_radius);
			set_box(m_sp_origin-r,
					m_sp_origin+r);
		}

		if (b.m_type == BOX)
		{
			bool first = false;
			eval_min_max(b.m_box_pmin, m_box_pmin, m_box_pmax, &first);
			eval_min_max(b.m_box_pmax, m_box_pmin, m_box_pmax, &first);
		}
		else
		{
			Vector3D r(b.m_sp_radius,
					b.m_sp_radius,
					b.m_sp_radius);
			bool first = false;
			eval_min_max(b.m_sp_origin-r, m_box_pmin, m_box_pmax, &first);
			eval_min_max(b.m_sp_origin+r, m_box_pmin, m_box_pmax, &first);
		}
	}
}
