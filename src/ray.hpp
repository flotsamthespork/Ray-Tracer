#ifndef __RAY_H__
#define __RAY_H__

#include "algebra.hpp"

class SceneObject;

class Ray {
private:
	Point3D		m_pos;
	Vector3D	m_dir;
	bool		m_normalized;
	double		m_power;
	double		m_refraction_index;
	SceneObject*	m_refraction_obj;

public:
	Ray();
	Ray(const Point3D &ray_pos,
			const Vector3D &ray_dir,
			const double power,
			const double refraction_index,
			SceneObject *refraction_obj,
			bool is_normalized = false);

	void normalize();

	const Point3D &get_pos() const;
	const Vector3D &get_dir() const;

	double get_power() const;
	double get_refraction_index() const;
	SceneObject *get_refraction_src() const;

	void transform(const Matrix4x4 *transform, Ray &ray) const;

};

#endif
