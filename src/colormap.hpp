#ifndef __COLORMAP_H__
#define __COLORMAP_H__


#include "algebra.hpp"
#include "image.hpp"

enum ColorMapType {
	CONSTANT,
	TEXTURE
};

class ColorMap {
private:

public:
	virtual Colour get_color(double *uv) = 0;

	virtual void bump(Vector3D &normal, Vector3D u_tangent, double *uv)
	{
		(void) normal;
		(void) u_tangent;
		(void) uv;
	}

	virtual bool compute_uv()
	{
		return false;
	}

	virtual void set_bump_magnitude(double mag)
	{
	}

	virtual ColorMapType get_type() = 0;
};


class ConstantColorMap : public ColorMap {
private:
	Colour m_color;
public:
	ConstantColorMap(const Colour &c);

	virtual Colour get_color(double*)
	{
		return m_color;
	}

	virtual ColorMapType get_type()
	{
		return CONSTANT;
	}

};

enum BlendMode {
	INTERPOLATE,
	NEAREST
};

enum WrapMode {
	REPEAT
};

class TextureColorMap : public ColorMap {
private:
	BlendMode m_blend;
	WrapMode m_wrap;
	Matrix4x4 m_transform;
	Image *m_img;
	int m_width, m_height;
	double m_bump_mag;

	Colour get_color(const int x,
			const int y,
			const bool red_only = false);
public:
	TextureColorMap(std::string &name);
	virtual ~TextureColorMap();

	virtual void bump(Vector3D &normal, Vector3D u_tangent, double *uv);

	void translate(double x, double y);
	void scale(double x, double y);
	void rotate(double angle);

	virtual Colour get_color(double *uv);

	virtual void set_bump_magnitude(double mag)
	{
		m_bump_mag = mag;
	}

	virtual ColorMapType get_type()
	{
		return TEXTURE;
	}

	virtual bool compute_uv()
	{
		return true;
	}
};

// TODO - TextureColorMap
//	* Has a shared image cache
//	* reference counting for images (so we can free their memory)
//	* UV transformations
//	* wrapping type(?)
//	* interpolation type(?)


#endif
