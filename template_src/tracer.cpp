
#include <vector>
#include "scene.hpp"

#include "tracer.hpp"


RayTracer::RayTracer(const Point3D &eye_pos,
		const Matrix4x4 &px_to_wcs,
		SceneNode *node, Image *img) :
	m_eye_pos(eye_pos),
	m_px_to_wcs(px_to_wcs),
	m_img(img)
{
	Scene scene;
	std::vector<SceneObject*> objects;
	Scene::make_scene(node, &scene);
}

