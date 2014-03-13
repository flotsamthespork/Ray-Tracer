
#include "job.hpp"

#include "tracer.hpp"


RayTracer::RayTracer(Image *img) :
	m_img(img)
{
}


const Image*
RayTracer::get_image() const
{
	return m_img;
}
