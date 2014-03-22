#ifndef __COLORMAP_H__
#define __COLORMAP_H__

#include "algebra.hpp"

enum ColorMapType {
	CONSTANT,
	TEXTURE
};

class ColorMap {
private:

public:
	virtual Colour& get_color(double u, double v) = 0;

	virtual bool compute_uv()
	{
		return false;
	}

	virtual ColorMapType get_type() = 0;
};


class ConstantColorMap : public ColorMap {
private:
	Colour m_color;
public:
	ConstantColorMap(const Colour &c);

	virtual Colour& get_color(double,double)
	{
		return m_color;
	}

	virtual ColorMapType get_type()
	{
		return CONSTANT;
	}

};

// TODO - TextureColorMap
//	* Has a shared image cache
//	* reference counting for images (so we can free their memory)
//	* UV transformations
//	* wrapping type(?)
//	* interpolation type(?)


#endif
