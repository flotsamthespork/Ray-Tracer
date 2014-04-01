#ifndef __TRACER_H__
#define __TRACER_H__

#include "algebra.hpp"
#include "image.hpp"
#include "intersection.hpp"
#include "scene.hpp"
#include "jitter.hpp"
#include "colormap.hpp"

struct JobData;

class RayTracer {
private:
	Scene *m_scene;
	IntersectionStrategy *m_intersect;

	ColorMap *m_background;
	Colour m_ambient;
	int m_num_threads;
	bool m_first_run;

	Camera *m_camera;
	Image *m_img;
	int m_img_width;
	int m_img_height;
	Matrix4x4 m_px_to_wcs;

	// Soft Shadow samples
	int m_ss_samples;

	bool ray(const Ray *ray, Colour &ray_color,
			JobData *data);
public:
	RayTracer(Scene *scene, IntersectionStrategy *is);
	virtual ~RayTracer();

	IntersectionStrategy *get_intersection_strategy() const;

	void set_background(ColorMap *map);
	void set_ambient(Colour &color);
	void set_num_threads(const int num_threads);
	void set_light_samples(const int samples);

	void render(const int cam_id, const std::string img_name,
			const int width, const int height);

	void trace_px(const int px,
			JobData *data);

	Colour light(const Ray *ray, IntersectionData &i,
			JobData *data);
};


#endif
