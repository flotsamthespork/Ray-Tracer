
#include <cmath>
#include "polyroots.hpp"
#include "scene.hpp"

#include "primitive.hpp"

static void sphere_uv_map(Vector3D pos,
		double *uv)
{
	pos.normalize();
	uv[0] = 0.5 - atan2(pos[2], pos[0]) / (2.0*M_PI);
	uv[1] = 0.5 - asin(pos[1]) / M_PI;
}

void
Sphere::intersection(const Ray *ray,
		IntersectionHelper *intersections)
{
	double roots[2];
	int n_roots;

	const Vector3D pc = ray->get_pos() - Point3D(0,0,0);

	double a = ray->get_dir().dot(ray->get_dir());
	double b = 2 * ray->get_dir().dot(pc);
	double c = pc.dot(pc) - m_radius*m_radius;

	n_roots = quadraticRoots(a,b,c, roots);
	for (int idx = 0; idx < n_roots; ++idx)
	{
		IntersectionData d;
		d.t = roots[idx];

		Point3D ip = ray->get_pos() + d.t*ray->get_dir();
		d.normal = ip - Point3D(0,0,0);
		d.u_tangent = d.normal.cross(Vector3D(0,1,0));
		sphere_uv_map(d.normal, d.uv);
		intersections->on_intersection(d);
	}
}

void
Box::intersection(const Ray *ray,
		IntersectionHelper *intersections)
{
}

void
Torus::intersection(const Ray *ray,
		IntersectionHelper *intersections)
{
	const Point3D &ray_pos = ray->get_pos();
	const Vector3D &ray_dir = ray->get_dir();
	const Vector3D ray_vpos(ray_pos[0], ray_pos[1], ray_pos[2]);

	const double inner_sq = m_inner_rad*m_inner_rad;
	const double outer_sq = m_outer_rad*m_outer_rad;

	const double a_a = ray_vpos.dot(ray_vpos);
	const double a_b = ray_vpos.dot(ray_dir);

	const double K = a_a - inner_sq - outer_sq;
	const double A = 4*a_b;
	const double B = 2*(2*a_b*a_b + K + 2*outer_sq*ray_dir[2]*ray_dir[2]);
	const double C = 4*(K*a_b + 2*outer_sq*ray_pos[2]*ray_dir[2]);
	const double D = K*K + 4*outer_sq*(ray_pos[2]*ray_pos[2] - inner_sq);

	double coeff[5];
	coeff[4] = 1;
	coeff[3] = A;
	coeff[2] = B;
	coeff[1] = C;
	coeff[0] = D;
	double roots[4];

	int n_roots = SolveQuartic(coeff, roots);
//	int n_roots = quarticRoots(A, B, C, D, roots);
	for (int idx = 0; idx < n_roots; ++idx)
	{
		IntersectionData d;
		d.t = roots[idx];

		Point3D ip = ray_pos + d.t*ray_dir;
		const double x2 = ip[0]*ip[0];
		const double y2 = ip[1]*ip[1];
		const double z2 = ip[2]*ip[2];
		const double mp = (x2+y2+z2) - inner_sq - outer_sq;

		d.normal[0] = 4*ip[0]*mp;
		d.normal[1] = 4*ip[1]*mp;
		d.normal[2] = 4*ip[2]*mp + 8*outer_sq*ip[2];

		d.u_tangent = d.normal.cross(Vector3D(0,1,0));

		double len = sqrt(x2 + y2);
		double x = len - m_outer_rad;
		d.uv[0] = (1.0 - (atan2(ip[1], ip[0]) + M_PI) / (2.0*M_PI));
		d.uv[1] = (atan2(ip[2], x) + M_PI) / (2.0*M_PI);

		intersections->on_intersection(d);
	}
}


