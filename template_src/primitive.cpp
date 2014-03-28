
#include <cmath>
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
		std::vector<IntersectionData> &intersections)
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

		Point3D ip = ray->get_pos() + d.t*ray->get_dir();
		d.normal = ip-m_pos;
		d.u_tangent = d.normal.cross(Vector3D(0,1,0));
		get_uv(ip, d.uv);
		intersections.push_back(d);
	}
}

void
Sphere::get_uv(Point3D point,
		double *uv)
{
	Vector3D v = point - m_pos;
	v.normalize();
	uv[0] = 0.5 - atan2(v[2], v[0])/(2.0*M_PI);
	uv[1] = 0.5 - asin(v[1])/M_PI;

//	uv[0] = acos(v[2]) / M_PI;
//	uv[1] = acos(v[0]/sin(M_PI*uv[0]))/(2.0*M_PI);

//	uv[0] = atan2(v[2], v[0]);
//	uv[1] = atan2(v[1], sqrt(v[0]*v[0]+v[2]*v[2]));
}


void
Box::intersection(const Ray *ray,
		std::vector<IntersectionData> &intersections)
{
}

void
Box::get_uv(Point3D point,
		double *uv)
{
}
void
Torus::intersection(const Ray *ray,
		std::vector<IntersectionData> &intersections)
{
	const Point3D &ray_pos = ray->get_pos();
	const Vector3D &ray_dir = ray->get_dir();
	const Vector3D ray_vpos(ray_pos[0], ray_pos[1], ray_pos[2]);

	const double inner_sq = m_inner_rad*m_inner_rad;
	const double outer_sq = m_outer_rad*m_outer_rad;
/*
	double a = ray_dir.dot(ray_dir);
	double b = 2*ray_dir.dot(ray_vpos);
	double c = ray_vpos.dot(ray_vpos) - inner_sq - outer_sq;

	const double A = a*a;
	const double B = 2*a*b;
	const double C = b*b + 2*a*c + 4*outer_sq*ray_dir[2]*ray_dir[2];
	const double D = 2*b*c + 8*outer_sq*ray_pos[2]*ray_dir[2];
	const double E = c*c + 4*outer_sq*ray_pos[2]*ray_pos[2] - 4*outer_sq*inner_sq;
*/

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
//	int n_roots = quarticSolver(coeff, roots);
//	int n_roots = quarticRoots(A, B, C, D, roots);
	int n_roots = SolveQuartic(coeff, roots);
//	int n_roots = quarticRoots(B/A, C/A, D/A, E/A, roots);

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

		get_uv(ip, d.uv);
		d.u_tangent = d.normal.cross(Vector3D(0,1,0));

		intersections.push_back(d);
	}
}

void
Torus::get_uv(Point3D p,
		double *uv)
{
	uv[0] = (1.0 - (atan2(p[1], p[0]) + M_PI) / (2.0*M_PI));
	double len = sqrt(p[0]*p[0] + p[1]*p[1]);

	double x = len - m_outer_rad;
	uv[1] = (atan2(p[2],x) + M_PI) / (2.0*M_PI);
}
