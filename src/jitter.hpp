#ifndef __JITTER_H__
#define __JITTER_H__

#include "algebra.hpp"

enum JitterShape {
	JITTER_POINT,
	JITTER_RECTANGLE,
//	JITTER_CIRCLE
};

class Jitter {
private:
	JitterShape m_shape;
	Point3D m_pos;
	Vector3D m_uvec;
	Vector3D m_vvec;

	double m_ustep;
	double m_vstep;

	double random() const;
public:
	Jitter();
	Jitter(const Point3D &pos,
			const Vector3D &uvec,
			const Vector3D &vvec);

	void set_shape(JitterShape shape);

	void set_pos(const Point3D &pos);
	void set_uvec(const Vector3D &uvec);
	void set_vvec(const Vector3D &vvec);
	void set_ustep(double ustep);
	void set_vstep(double vstep);

	void transform(const Matrix4x4 &transform);

	JitterShape get_shape() const;

	Point3D jitter(double u, double v) const;
};

#endif
