#ifndef __TRACER_H__
#define __TRACER_H__

#include <vector>
#include "image.hpp"

class RayTracer {
private:
	Image *m_img;
public:
	RayTracer(Image *img);

	const Image *get_image() const;
};


#endif
