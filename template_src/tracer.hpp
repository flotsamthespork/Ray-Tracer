#ifndef __TRACER_H__
#define __TRACER_H__

#include "algebra.hpp"
#include "image.hpp"
#include "intersection.hpp"
#include "scene.hpp"

class RayTracer {
private:
	Scene *m_scene;
	IntersectionStrategy *m_intersect;

	Colour m_ambient;
	int m_num_threads;

	Camera *m_camera;
public:
	RayTracer(Scene *scene, IntersectionStrategy *is);
	virtual ~RayTracer();

	void set_ambient(Colour &color);
	void set_num_threads(const int num_threads);

	void render(const int cam_id, const std::string img_name,
			const int width, const int height);
};


#endif
