
#include <cmath>
#include <float.h>
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
	double t1, t2;
	double t[2];
	t[0] = -DBL_MAX;
	t[1] = DBL_MAX;

	const Point3D &ray_pos = ray->get_pos();
	const Vector3D &ray_dir = ray->get_dir();

	const Point3D pnear = Point3D(0,0,0);
	const Point3D pfar(m_size,m_size,m_size);

	bool intersect = true;
	for (int i = 0; intersect && i < 3; ++i)
	{
		if (ray_dir[i] == 0)
		{
			if (ray_pos[i] < pnear[i] || ray_pos[i] > pfar[i])
				intersect = false;
		}
		else
		{
			t1 = (pnear[i] - ray_pos[i]) / ray_dir[i];
			t2 = (pfar[i] - ray_pos[i]) / ray_dir[i];
			if (t1 > t2)
			{
				double tmp = t1;
				t1 = t2;
				t2 = tmp;
			}
			t[0] = std::max(t[0], t1);
			t[1] =  std::min(t[1], t2);
			if (t[0] > t[1])
				intersect = false;
			if (t[1] < 0)
				intersect = false;
		}
	}

	if (intersect)
	{
		for (int i = 0; i < 2; ++i)
		{
			IntersectionData d;
			d.t = t[i];

//			data.normal = -1*norm;
//			data.u_tangent = Vector3D(1,0,0);
//			data.uv[0] = 0.5 + 0.5*ip[0]/m_radius;
//			data.uv[1] = 0.5 - 0.5*ip[1]/m_radius;
			const Point3D ip = ray_pos + d.t*ray_dir;

#define EPSILON 0.0000001

			if (std::abs(ip[0]-pnear[0]) < EPSILON)
			{
				d.normal = Vector3D(-1,0,0);
				d.u_tangent = Vector3D(0,0,1);
				d.uv[0] = (ip[2])/m_size;
				d.uv[1] = (m_size-ip[1])/m_size;
			}
			else if (std::abs(ip[0]-pfar[0]) < EPSILON)
			{
				d.normal = Vector3D(1,0,0);
				d.u_tangent = Vector3D(0,0,-1);
				d.uv[0] = (m_size-ip[2])/m_size;
				d.uv[1] = (m_size-ip[1])/m_size;
			}
			else if (std::abs(ip[1]-pnear[1]) < EPSILON)
			{
				d.normal = Vector3D(0,-1,0);
				d.u_tangent =Vector3D(1,0,0);
				d.uv[0] = (m_size-ip[0])/m_size;
				d.uv[1] = (m_size-ip[2])/m_size;
			}
			else if (std::abs(ip[1]-pfar[1]) < EPSILON)
			{
				d.normal = Vector3D(0,1,0);
				d.u_tangent =Vector3D(-1,0,0);
				d.uv[0] = (ip[0])/m_size;
				d.uv[1] = (m_size-ip[2])/m_size;
			}
			else if (std::abs(ip[2]-pnear[2]) < EPSILON)
			{
				d.normal = Vector3D(0,0,-1);
				d.u_tangent = Vector3D(1,0,0);
				d.uv[0] = (m_size-ip[0])/m_size;
				d.uv[1] = (m_size-ip[1])/m_size;
			}
			else
			{
				d.normal = Vector3D(0,0,1);
				d.u_tangent = Vector3D(-1,0,0);
				d.uv[0] = (ip[0])/m_size;
				d.uv[1] = (m_size-ip[1])/m_size;
			}

			intersections->on_intersection(d);
		}
	}
}

void
Cylinder::intersection(const Ray *ray,
		IntersectionHelper *intersections)
{
	double roots[2];
	int n_roots;

	const Point3D &p = ray->get_pos();
	const Vector3D &d = ray->get_dir();

	const double r_sq = m_radius*m_radius;

	const double a = d[0]*d[0] + d[1]*d[1];
	const double b = 2*(d[0]*p[0] + d[1]*p[1]);
	const double c = p[0]*p[0] + p[1]*p[1] - r_sq;

	int num_i = 0;

	n_roots = quadraticRoots(a,b,c, roots);
	for (int i = 0; i < n_roots; ++i)
	{
		Point3D ip = p + roots[i]*d;
		if (std::abs(ip[2]) <= m_length/2)
		{
			IntersectionData data;
			data.t = roots[i];

			data.normal = ip - Point3D(0, 0, ip[2]);
			data.u_tangent = Vector3D(0, 0, 1);

			data.uv[0] = 0.5 + ip[2]/m_length;
			data.uv[1] = 0.5 - atan2(ip[1], ip[0]) / (2.0*M_PI);
		
			intersections->on_intersection(data);
			num_i++;
		}
	}

	if (num_i >= 2)
		return;

	const Vector3D vp = p - Point3D(0,0,0);

	{
		Vector3D norm(0,0,1);
		Point3D center(0,0,-m_length/2);
		const double t = (-std::abs(center[2])-(norm.dot(vp))) / norm.dot(d);
		Point3D ip = p + t*d;
		Vector3D v = ip - center;
		if (v.dot(v) <= r_sq)
		{
			IntersectionData data;
			data.t = t;
			data.normal = -1*norm;
			data.u_tangent = Vector3D(1,0,0);
			data.uv[0] = 0.5 + 0.5*ip[0]/m_radius;
			data.uv[1] = 0.5 - 0.5*ip[1]/m_radius;

			intersections->on_intersection(data);
			num_i++;
		}
	}

	if (num_i < 2)
	{
		Vector3D norm(0,0,-1);
		Point3D center(0,0,m_length/2);
		const double t = (-std::abs(center[2])-(norm.dot(vp))) / norm.dot(d);
		Point3D ip = p + t*d;
		Vector3D v = ip - center;
		if (v.dot(v) <= r_sq)
		{
			IntersectionData data;
			data.t = t;
			data.normal = -1*norm;
			data.u_tangent = Vector3D(1,0,0);
			data.uv[0] = 0.5 + 0.5*ip[0]/m_radius;
			data.uv[1] = 0.5 - 0.5*ip[1]/m_radius;

			intersections->on_intersection(data);
			num_i++;
		}
	}


}

void
Cone::intersection(const Ray *ray,
		IntersectionHelper *intersections)
{
	double roots[2];
	int n_roots;

	const Point3D &p = ray->get_pos();
	const Vector3D &d = ray->get_dir();

	const double r_sq = m_rad_scale*m_rad_scale;

	const double a = d[0]*d[0] + d[1]*d[1] - r_sq*d[2]*d[2];
	const double b = 2*(d[0]*p[0] + d[1]*p[1] - r_sq*d[2]*p[2]);
	const double c = p[0]*p[0] + p[1]*p[1] - r_sq*p[2]*p[2];

	int num_i = 0;

	n_roots = quadraticRoots(a,b,c, roots);
	for (int i = 0; i < n_roots; ++i)
	{
		Point3D ip = p + roots[i]*d;
		if (ip[2] >= 0 && ip[2] <= m_length)
		{
			IntersectionData data;
			data.t = roots[i];

			data.u_tangent = ip - Point3D(0,0,0);
			data.normal = data.u_tangent.cross(Vector3D(0,0,-1));
			data.normal = data.normal.cross(data.u_tangent);

			data.uv[0] = ip[2]/m_length;
			data.uv[1] = 0.5 - atan2(ip[1], ip[0]) / (2.0*M_PI);
		
			intersections->on_intersection(data);
			num_i++;
		}
	}

	if (num_i >= 2)
		return;

	const Vector3D vp = p - Point3D(0,0,0);

	{
		Vector3D norm(0,0,-1);
		Point3D center(0,0,m_length);
		const double t = (-std::abs(center[2])-(norm.dot(vp))) / norm.dot(d);
		Point3D ip = p + t*d;
		Vector3D v = ip - center;
		if (v.dot(v) <= (m_rad_scale*m_rad_scale*m_length*m_length))
		{
			IntersectionData data;
			data.t = t;
			data.normal = -1*norm;
			data.u_tangent = Vector3D(1,0,0);
			data.uv[0] = 0.5 + 0.5*ip[0]/(m_rad_scale*m_length);
			data.uv[1] = 0.5 - 0.5*ip[1]/(m_rad_scale*m_length);

			intersections->on_intersection(data);
			num_i++;
		}
	}
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


