
#include "algebra.hpp"

#include "material.hpp"

static ConstantColorMap default_color(Colour(0.5));

Material::Material() :
	m_diffuse(&default_color),
	m_specular(0),
	m_bump(0),
	m_shininess(0),
	m_refraction_index(0),
	m_isdielectric(false),
	m_mirror_coeff(0)
{
}

Material::~Material()
{
}

ColorMap*
Material::get_diffuse()
{
	return m_diffuse;
}

ColorMap*
Material::get_specular()
{
	return m_specular;
}

ColorMap*
Material::get_bump()
{
	return m_bump;
}

double
Material::get_shininess()
{
	return m_shininess;
}

double
Material::get_refraction_index()
{
	return m_refraction_index;
}

bool
Material::is_dielectric()
{
	return m_isdielectric;
}

double
Material::get_mirror_coefficient()
{
	return m_mirror_coeff;
}

void
Material::set_diffuse(ColorMap *map)
{
	m_diffuse = map;
}

void
Material::set_specular(ColorMap *map)
{
	m_specular = map;
}

void
Material::set_bump(ColorMap *map)
{
	m_bump = map;
}

void
Material::set_shininess(double shiny)
{
	m_shininess = shiny;
}

void
Material::set_refraction_index(double refraction_index,
		bool is_dielectric)
{
	m_refraction_index = refraction_index;
	m_isdielectric = is_dielectric;
}

void
Material::set_mirror_coefficient(double c)
{
	m_mirror_coeff = c;
}

