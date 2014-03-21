#ifndef __TRACER_H__
#define __TRACER_H__

#include "algebra.hpp"
#include "image.hpp"
#include "intersection.hpp"
#include "scene_tree.hpp"

class RayTracer {
private:
	const Point3D m_eye_pos;
	const Matrix4x4 m_px_to_wcs;
	Image *m_img;
public:
//	RayTracer(Scene *scene);

//	void set_ambient(Colour &color);

//	void trace_img(int cam_id,

	RayTracer(const Point3D &eye_pos,
			const Matrix4x4 &px_to_wcs,
			SceneNode *node,
			Image *img);

	// TODO - pass in an IntersectionStrategy
};


#endif
