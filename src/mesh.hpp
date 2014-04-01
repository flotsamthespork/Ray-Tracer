#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include "ray.hpp"
#include "primitive.hpp"
#include "intersection.hpp"

typedef std::vector<int> Face;
typedef Face::const_iterator FaceItr;

class Polygon : public Primitive {
public:
	Polygon(const std::vector<Point3D> &verts,
		const std::vector<Point2D> &uvs,
		const std::vector<Vector3D> &norms,
		const Face *vert_i,
		const Face *uv_i,
		const Face *norm_i);

	virtual bool intersection(const Ray *ray,
			IntersectionHelper *intersections);

	virtual void get_bounds(Bounds &b);
private:
	std::vector<Point3D>	m_verts;
	std::vector<Point2D>	m_uvs;
	std::vector<Vector3D>	m_normals;

	Vector3D		m_normal;
};



class Mesh : public Primitive {
public:
	Mesh(const std::vector<Point3D>& verts,
			const std::vector<Point2D> &uv,
			const std::vector<Vector3D> &norms,
			const std::vector<Face>& face_vs,
			const std::vector<Face>& face_vts,
			const std::vector<Face>& face_norms);

	virtual ~Mesh();

	virtual bool intersection(const Ray *ray,
			IntersectionHelper *intersections);

	virtual void get_bounds(Bounds &b);

	virtual void finish(IntersectionStrategy *is);

private:
	IntersectionStrategy	*m_intersect;
	std::vector<PrimitiveObject*> m_polys;

	Matrix4x4 m_bounds_trans;
	Matrix4x4 m_bounds_itrans;
	Box m_bounds;
};

#endif
