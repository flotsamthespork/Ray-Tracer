#ifndef __BOUNDS_H__
#define __BOUNDS_H__

#include "algebra.hpp"

enum BoundsType {
	UNSET,
	BOX,
	SPHERE
};


class Bounds {
private:
	BoundsType m_type;

	Point3D m_box_pmin;
	Point3D m_box_pmax;

	Point3D m_sp_origin;
	double m_sp_radius;

public:
	Bounds();
	Bounds(const Bounds &b);
	virtual ~Bounds();

	BoundsType get_type()
	{
		return m_type;
	}

	double get_left();
	double get_right();
	double get_top();
	double get_bottom();
	double get_front();
	double get_back();

	void set_box(const Point3D &pmin,
			const Point3D &pmax);

	void set_sphere(const Point3D &origin,
			const double radius);

	void transform(const Matrix4x4 &transform);

	void merge(const Bounds &b);

	void copy(const Bounds &b);
};

#endif
