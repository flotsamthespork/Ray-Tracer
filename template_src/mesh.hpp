#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include "ray.hpp"
#include "primitive.hpp"

typedef std::vector<int> Face;
typedef Face::const_iterator FaceItr;

class Polygon : public Primitive {
public:
	Polygon(const std::vector<Point3D> &verts,
		const std::vector<Point2D> &uvs,
		const Face &polygon);

	virtual void intersection(const Ray *ray,
			std::vector<IntersectionData> &intersections);

	virtual void get_uv(Point3D point,
			double *uv);

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

	virtual void intersection(const Ray *ray,
			std::vector<IntersectionData> &intersections);

	virtual void get_uv(Point3D point,
			double *uv);

private:
	std::vector<Polygon>	m_polys;

	// TODO - bounding box (?) or just an IntersectionStrategy
};

#endif
