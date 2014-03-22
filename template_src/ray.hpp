#ifndef __RAY_H__
#define __RAY_H__

#include "algebra.hpp"

class Ray {
private:
	Point3D		m_pos;
	Vector3D	m_dir;
	bool		m_normalized;

public:
	Ray();
	Ray(const Point3D &ray_pos,
			const Vector3D &ray_dir,
			bool is_normalized = false);

	void normalize();

	const Point3D &get_pos() const;
	const Vector3D &get_dir() const;

	void transform(const Matrix4x4 &transform, Ray &ray) const;

};

#endif
