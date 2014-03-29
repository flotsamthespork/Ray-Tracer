#ifndef CS488_MATERIAL_HPP
#define CS488_MATERIAL_HPP

#include "colormap.hpp"


class Material {
private:
	ColorMap *m_diffuse;
	ColorMap *m_specular;
	ColorMap *m_bump;

	double m_shininess;

	double m_refraction_index;
	bool m_isdielectric;

	double m_mirror_coeff;

public:
	Material();
	~Material();

	ColorMap *get_diffuse();
	ColorMap *get_specular();
	ColorMap *get_bump();
	double get_shininess();
	double get_refraction_index();
	bool is_dielectric();
	double get_mirror_coefficient();

	void set_diffuse(ColorMap *map);
	void set_specular(ColorMap *map);
	void set_bump(ColorMap *map);
	void set_shininess(double shiny);
	void set_refraction_index(double refraction_index,
			bool is_dielectric);
	void set_mirror_coefficient(double c);
};



// class ColorMap; - solid color, texture (w/ posibilitiy of UV transforms)

// class Material;
//	- diffuse color	(ColorMap)
//	- bump map	(ColorMap)
//	- specular color (ColorMap)
//	- shinyness	(double)
//	- refraction
//		- refraction index	(double)
//		- is dielectric		(bool)
//			* generates a coefficient for reflection
//	- reflection
//		- reflection coeff	(double)
//		* 

// For refraction
//	* get refracted color
//	* multiply this color by the diffuse color

















#endif
