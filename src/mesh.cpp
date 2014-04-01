
#include "algebra.hpp"
#include "mat3.hpp"
#include "scene.hpp"

#include "mesh.hpp"

static Point3D
find_mid(const Point3D &a, const Point3D &b)
{
	return Point3D((a[0]+b[0])/2,
			(a[1]+b[1])/2,
			(a[2]+b[2])/2);
}

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
	if (gamma < 0 || (beta+gamma) >= 1)
		return false;

	return true;
}

Polygon::Polygon(const std::vector<Point3D> &verts,
		const std::vector<Point2D> &uvs,
		const Face *vert_i,
		const Face *uv_i)
{

	for (int i = 0; i < vert_i->size(); ++i)
	{
		m_verts.push_back(verts[(*vert_i)[i]]);
		if (uv_i && i < uv_i->size())
		{
			m_uvs.push_back(uvs[(*uv_i)[i]]);
		}
		else
			m_uvs.push_back(Point2D(0,0));
	}
/*
	for (FaceItr i = vert_i->begin();
			i != vert_i->end(); ++i)
	{
		m_verts.push_back(verts[*i]);
		if (uv_i && uv_i->size() > *i)
		{
			m_uvs.push_back(uvs[(*uv_i)[*i]]);
			std::cout << (*uv_i)[*i] << std::endl;
//			std::cout <<uvs[(*uv_i)[*i]] << std::endl;
		}
		else
			m_uvs.push_back(Point2D(0,0));
	}
*/
	m_normal = (m_verts[1] - m_verts[0]).cross(m_verts[2] - m_verts[0]);
}

bool
Polygon::intersection(const Ray *ray,
		IntersectionHelper *intersections)
{

	for (int i = 1; i < m_verts.size()-1; ++i)
	{
		double beta, gamma;
		if (triangle_intersection(ray, m_verts[0], m_verts[i], m_verts[i+1],
					beta, gamma))
		{
			if (!intersections)
				return true;

			IntersectionData d;
			d.normal = m_normal;
			d.u_tangent = m_verts[i+1] - m_verts[0];

			Point2D uv0 = m_uvs[0];
			Point2D uv1 = m_uvs[i];
			Point2D uv2 = m_uvs[i+1];
			d.uv[0] = (1-beta-gamma)*uv0[0] + beta*uv1[0] + gamma*uv2[0];
			d.uv[1] = (1-beta-gamma)*uv0[1] + beta*uv1[1] + gamma*uv2[1];

			d.t = (m_verts[0]-ray->get_pos()).dot(m_normal) /
				(m_normal.dot(ray->get_dir()));

			intersections->on_intersection(d);
			return true;
		}
	}
	return false;
}

void
Polygon::get_bounds(Bounds &b)
{
	bool first = true;
	Point3D min, max;
	for (int i = 0; i < m_verts.size(); ++i)
		eval_min_max(m_verts[i], min, max, &first);
	b.set_box(min, max);
}


Mesh::Mesh(const std::vector<Point3D>& verts,
		const std::vector<Point2D> &uv,
		const std::vector<Face>& face_vs,
		const std::vector<Face>& face_vts) :
	m_bounds(1),
	m_intersect(0)
{
	for (int i = 0; i < face_vs.size(); ++i)
	{
		const Face *face_vert = &face_vs[i];
		const Face *face_tex = 0;
		if (face_vts.size() >= i)
			face_tex = &face_vts[i];
		Polygon *p = new Polygon(verts, uv, face_vert, face_tex);
		PrimitiveObject *o = new PrimitiveObject(i, NULL, p, NULL);
		m_polys.push_back(o);
	}

	bool first = true;
	Point3D min, max;

	for (int i = 0; i < verts.size(); ++i)
		eval_min_max(verts[i], min, max, &first);

	Vector3D size = max-min;
	if (size[0] == 0) size[0] = 0.0000001;
	if (size[1] == 0) size[1] = 0.0000001;
	if (size[2] == 0) size[2] = 0.0000001;

	m_bounds_trans = translation(min-Point3D(0,0,0)) * scaling(size);
	m_bounds_itrans = m_bounds_trans.invert();
}

Mesh::~Mesh()
{
	for (int i = 0; i < m_polys.size(); ++i)
	{
		delete m_polys[i]->get_primitive();
		delete m_polys[i];
	}
	delete m_intersect;
}

void
Mesh::finish(IntersectionStrategy *is)
{
	if (m_intersect)
		delete m_intersect;
	m_intersect = get_strategy(is->get_params());
	for (int i = 0; i < m_polys.size(); ++i)
		m_intersect->add_object(m_polys[i]);
	m_intersect->finish();
}

void
Mesh::get_bounds(Bounds &b)
{
	m_bounds.get_bounds(b);
	b.transform(m_bounds_trans);
}

bool
Mesh::intersection(const Ray *ray,
		IntersectionHelper *intersections)
{
	Ray trans_ray;
	ray->transform(&m_bounds_itrans, trans_ray);
	if (!m_bounds.intersection(&trans_ray, NULL))
		return false;

	Intersection *i = intersections->get_intersection();
	IntersectionCache *my_cache = intersections->get_cache(m_polys.size());
	if (my_cache)
		my_cache->increment_cache_version();
	IntersectionCache *old_cache = i->swap_cache(my_cache);

	std::vector<IntersectionData> inters;
	m_intersect->get_intersections(ray, inters, i);

	bool intersect = false;
	for (int j = 0; j < inters.size(); ++j)
	{
		intersections->on_intersection(inters[j]);
		intersect = true;
	}

	i->swap_cache(old_cache);

	return intersect;
}


