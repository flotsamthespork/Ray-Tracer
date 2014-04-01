
#include <iostream>
#include "image_cache.hpp"

#include "colormap.hpp"

ConstantColorMap::ConstantColorMap(const Colour &c) :
	m_color(c)
{
}


TextureColorMap::TextureColorMap(std::string &name) :
	m_blend(INTERPOLATE),
	m_wrap(REPEAT),
	m_img(ImageCache::acquire_image(name)),
	m_width(0),
	m_height(0),
	m_bump_mag(2)
{
	if (m_img)
	{
		m_width = m_img->width();
		m_height = m_img->height();
	}
}

TextureColorMap::~TextureColorMap()
{
}

Colour
TextureColorMap::get_color(const int x,
		const int y,
		const bool red_only)
{
	int xx = x;
	int yy = y;
	switch (m_wrap)
	{
	case REPEAT:
		xx = xx % m_width;
		if (xx < 0)
			xx += m_width;
		yy = yy % m_height;
		if (yy < 0)
			yy += m_height;
		break;
	}

	if (red_only)
		return Colour((*m_img)(xx,yy,0),0,0);

	return Colour((*m_img)(xx,yy,0),
			(*m_img)(xx,yy,1),
			(*m_img)(xx,yy,2));
}

void
TextureColorMap::bump(Vector3D &normal,
		Vector3D u_tangent,
		double *_uv)
{
	if (m_img)
	{
		Point3D uv = m_transform * Point3D(_uv[0], _uv[1], 0);

		uv[0] *= m_width;
		uv[1] *= m_height;

		int x = (int)uv[0];
		int y = (int)uv[1];

		Colour l = get_color(x-1,y,true);
		Colour r = get_color(x+1,y,true);
		Colour t = get_color(x,y-1,true);
		Colour b = get_color(x,y+1,true);

		double gx = m_bump_mag * (l.R() - r.R());
		double gy = m_bump_mag * (t.R() - b.R());

		Vector3D v_tangent = normal.cross(u_tangent);

		u_tangent.normalize();
		v_tangent.normalize();

		normal = normal + gx * u_tangent;
		normal = normal + gy * v_tangent;

//		normal[0] += gx;
//		normal[1] += gy;

		/*
		if (m_blend == NEAREST)
		{
			// TODO
		}
		else
		{
		}
		*/
	}
}

Colour
TextureColorMap::get_color(double *_uv)
{
	Colour c(0);
	if (m_img)
	{
		Point3D uv = m_transform * Point3D(_uv[0], _uv[1], 0);

		uv[0] *= m_width;
		uv[1] *= m_height;

		int x = (int)uv[0];
		int y = (int)uv[1];


		if (m_blend == NEAREST)
			c = get_color(x, y);
		else
		{
			double pu = uv[0] - x;
			double pv = uv[1] - y;

			Colour c00 = get_color(x,y);
			Colour c10 = get_color(x+1,y);
			Colour c01 = get_color(x,y+1);
			Colour c11 = get_color(x+1,y+1);

			c = (1-pu)*(1-pv)*c00 +
				pu*(1-pv)*c10 +
				(1-pu)*pv*c01 +
				pu*pv*c11;
		}
	}
	return c;
}

void
TextureColorMap::translate(double x, double y)
{
	Vector3D v(x,y,0);
	m_transform = m_transform * translation(v);
}


void
TextureColorMap::scale(double x, double y)
{
	Vector3D v(x,y,1);
	m_transform = m_transform * scaling(v);
}

void
TextureColorMap::rotate(double angle)
{
	m_transform = m_transform * rotation(angle, 'z');
}
