#ifndef __MAT3_H__
#define __MAT3_H__

#include "algebra.hpp"

class Matrix3x3 {
private:
	double m[9];
public:
	Matrix3x3(const Vector3D &col1,
			const Vector3D &col2,
			const Vector3D &col3)
	{
		m[0] = col1[0];
		m[1] = col1[1];
		m[2] = col1[2];
		m[3] = col2[0];
		m[4] = col2[1];
		m[5] = col2[2];
		m[6] = col3[0];
		m[7] = col3[1];
		m[8] = col3[2];
	}

	double det()
	{
		return m[0]*(m[4]*m[8] - m[5]*m[7]) -
			m[3] * (m[1]*m[8] - m[2]*m[7]) +
			m[6] * (m[1]*m[5] - m[2]*m[4]);
	}

};

#endif
