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
		const Face &polygon);

	virtual void intersection(const Ray *ray,
			IntersectionHelper *intersections);

private:
	std::vector<Point3D>	m_verts;
	std::vector<Point2D>	m_uvs;

	Vector3D		m_normal;
};



class Mesh : public Primitive {
public:
	Mesh(const std::vector<Point3D>& verts,
			const std::vector<Point2D> &uv,
			const std::vector<Face>& faces);

	virtual ~Mesh();

	virtual void intersection(const Ray *ray,
			IntersectionHelper *intersections);

private:
	IntersectionStrategy	*m_intersect;
	std::vector<PrimitiveObject*> m_polys;

	// TODO - bounding box (?) or just an IntersectionStrategy
};

#endif
