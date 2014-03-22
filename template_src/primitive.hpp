#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include <list>
#include "algebra.hpp"
#include "ray.hpp"

class Primitive {
public:
	virtual ~Primitive() {}

	virtual void intersection(const Ray *ray,
			std::list<IntersectionData> &intersections) = 0;
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
			std::list<IntersectionData> &intersections);

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
			std::list<IntersectionData> &intersections);

private:
	Point3D m_pos;
	double m_size;
};

#endif
