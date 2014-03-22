
#include "polyroots.hpp"

#include "primitive.hpp"

/*
void
Sphere::intersect(const Ray *ray, Intersection *i)
{
	double roots[2];
	int n_roots;

	const Vector3D pc = ray->get_pos() - m_pos;

	double a = ray->get_dir().dot(ray->get_dir());
	double b = 2 * ray->get_dir().dot(pc);
	double c = pc.dot(pc) - m_radius*m_radius;

	n_roots = quadraticRoots(a,b,c, roots);
	for (int idx = 0; idx < n_roots; ++idx)
	{
//		if (i->
	}
}


void
Box::intersect(const Ray *ray, Intersection *i)
{
	// TODO
}
*/
void
Sphere::intersection(const Ray *ray,
		std::list<IntersectionData> &intersections)
{
	double roots[2];
	int n_roots;

	const Vector3D pc = ray->get_pos() - m_pos;

	double a = ray->get_dir().dot(ray->get_dir());
	double b = 2 * ray->get_dir().dot(pc);
	double c = pc.dot(pc) - m_radius*m_radius;

	n_roots = quadraticRoots(a,b,c, roots);
	for (int idx = 0; idx < n_roots; ++idx)
	{
		IntersectionData d;
		d.t = roots[idx];

		Point3D ip = ray->get_pos() + roots[idx]*ray->get_dir();
		d.normal = ip-m_pos;
		intersections.push_back(d);
	}
}


void
Box::intersection(const Ray *ray,
		std::list<IntersectionData> &intersections)
{
}
