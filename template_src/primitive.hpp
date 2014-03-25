#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include <vector>
#include "algebra.hpp"
#include "ray.hpp"

class Primitive {
public:
	virtual ~Primitive() {}

	virtual void intersection(const Ray *ray,
			std::vector<IntersectionData> &intersections) = 0;

	virtual void get_uv(Point3D point,
			double *uv) = 0;
};


class Sphere : public Primitive {
public:
	Sphere(const Point3D& pos, double radius)
		: m_pos(pos), m_radius(radius)
	{
	}
	virtual ~Sphere()
	{}

	virtual void intersection(const Ray *ray,
			std::vector<IntersectionData> &intersections);

	virtual void get_uv(Point3D point,
			double *uv);
private:
	Point3D m_pos;
	double m_radius;
};

class Box : public Primitive {
public:
	Box(const Point3D& pos, double size)
		: m_pos(pos), m_size(size)
	{
	}
	
	virtual ~Box()
	{}

	virtual void intersection(const Ray *ray,
			std::vector<IntersectionData> &intersections);

	virtual void get_uv(Point3D point,
			double *uv);
private:
	Point3D m_pos;
	double m_size;
};


class Torus : public Primitive {
public:
	Torus(double inner_rad, double outer_rad) :
		m_inner_rad(inner_rad),
		m_outer_rad(outer_rad)
	{
	}

	virtual ~Torus()
	{
	}

	virtual void intersection(const Ray *ray,
			std::vector<IntersectionData> &intersections);

	virtual void get_uv(Point3D point,
			double *uv);

private:
	double m_inner_rad;
	double m_outer_rad;
};

#endif
