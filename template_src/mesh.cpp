
#include "algebra.hpp"
#include "mat3.hpp"

#include "mesh.hpp"


/*
static void
eval_min_max(const Point3D &p, Point3D &min, Point3D &max, bool *first)
{
	if (*first)
	{
		*first = false;
		min = p;
		max = p;
	}
	else
	{
		min[0] = std::min(min[0], p[0]);
		min[1] = std::min(min[1], p[1]);
		min[2] = std::min(min[2], p[2]);
		max[0] = std::max(max[0], p[0]);
		max[1] = std::max(max[1], p[1]);
		max[2] = std::max(max[2], p[2]);
	}
}

static Point3D
find_mid(const Point3D &a, const Point3D &b)
{
	return Point3D((a[0]+b[0])/2,
			(a[1]+b[1])/2,
			(a[2]+b[2])/2);
}
*/

static bool
triangle_intersection(const Ray *ray,
		const Point3D &p0,
		const Point3D &p1,
		const Point3D &p2,
		double &beta,
		double &gamma)
{
	Vector3D v1 = p1 - p0;
	Vector3D v2 = p2 - p0;
	Vector3D r = ray->get_pos() - p0;

	double d = Matrix3x3(v1,v2,ray->get_dir()).det();
	double d1 = Matrix3x3(r,v2,ray->get_dir()).det();
	beta = d1/d;
	if (beta < 0)
		return false;
	double d2 = Matrix3x3(v1, r, ray->get_dir()).det();
	gamma = d2/d;
	if (gamma < 0 || (beta+gamma) > 1)
		return false;

	return true;
}

Polygon::Polygon(const std::vector<Point3D> &verts,
		const std::vector<Point2D> &uvs,
		const Face &polygon)
{

	for (FaceItr i = polygon.begin();
			i != polygon.end(); ++i)
	{
		m_verts.push_back(verts[*i]);
		if (uvs.size() > *i)
			m_uvs.push_back(uvs[*i]);
		else
			m_uvs.push_back(Point2D(0,0));
	}

	m_normal = (verts[1] - m_verts[0]).cross(m_verts[m_verts.size()-1] - m_verts[0]);
}

void
Polygon::intersection(const Ray *ray,
		std::vector<IntersectionData> &intersections)
{

	for (int i = 1; i < m_verts.size()-1; ++i)
	{
		double beta, gamma;
		if (triangle_intersection(ray, m_verts[0], m_verts[i], m_verts[i+1],
					beta, gamma))
		{
			IntersectionData d;

			d.normal = m_normal;

			// TODO - figure this one out.
			d.u_tangent = Vector3D(1,0,0);

			Point2D uv0 = m_uvs[0];
			Point2D uv1 = m_uvs[i];
			Point2D uv2 = m_uvs[i+1];
			d.uv[0] = (1-beta-gamma)*uv0[0] + beta*uv1[0] + gamma*uv2[0];
			d.uv[1] = (1-beta-gamma)*uv0[1] + beta*uv1[1] + gamma*uv2[1];

			if (d.uv[0] < 0 || d.uv[0] > 1)
				std::cout << d.uv[0] << std::endl;

			d.t = (m_verts[0]-ray->get_pos()).dot(m_normal) /
				(m_normal.dot(ray->get_dir()));

			intersections.push_back(d);
			break;
		}
	}
}

void
Polygon::get_uv(Point3D point,
		double *uv)
{
}


Mesh::Mesh(const std::vector<Point3D>& verts,
		const std::vector<Point2D> &uv,
		const std::vector<Face>& faces)
{
	for (int i = 0; i < faces.size(); ++i)
	{
		Polygon p(verts, uv, faces[i]);
		m_polys.push_back(p);
	}
}

void
Mesh::intersection(const Ray *ray,
		std::vector<IntersectionData> &intersections)
{

	for (int i = 0; i < m_polys.size(); ++i)
	{
		m_polys[i].intersection(ray, intersections);
	}
}

void
Mesh::get_uv(Point3D point,
		double *uv)
{
}

/*
class Mesh : public Primitive {
public:

	virtual void intersection(const Ray *ray,
			std::vector<IntersectionData> &intersections);

	virtual void get_uv(Point3D point,
			double *uv);

private:
	std::vector<Point3D>	m_verts;
	std::vector<Point2D>	m_uvs;
	std::vector<Face>	m_faces;
	std::vector<Point3D>	m_face_centers;
	*/
