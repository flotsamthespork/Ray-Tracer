#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include <vector>
#include "algebra.hpp"
#include "ray.hpp"

class IntersectionHelper;

class Primitive {
public:
	virtual ~Primitive() {}

	virtual void intersection(const Ray *ray,
			IntersectionHelper *intersections) = 0;
};


class Sphere : public Primitive {
public:
	Sphere(double radius)
		: m_radius(radius)
	{
	}
	virtual ~Sphere()
	{}

	virtual void intersection(const Ray *ray,
			IntersectionHelper *intersections);
private:
	double m_radius;
};

class Box : public Primitive {
public:
	Box(double size)
		: m_size(size)
	{
	}
	
	virtual ~Box()
	{}

	virtual void intersection(const Ray *ray,
			IntersectionHelper *intersections);
private:
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
			IntersectionHelper *intersections);

private:
	double m_inner_rad;
	double m_outer_rad;
};

#endif
