#ifndef __SCENE_H__
#define __SCENE_H__

#include <vector>

#include "algebra.hpp"
#include "material.hpp"
#include "primitive.hpp"
#include "scene_tree.hpp"


class Scene;


class SceneObject {
private:
	const int m_id;
	const Matrix4x4 m_trans;
	const Matrix4x4 m_itrans;
	Primitive *const m_prim;
	Material *const m_material;

	SceneObject(const int id,
			const Matrix4x4 &trans,
			Primitive *primitive,
			Material *material);

public:
	virtual ~SceneObject();

	int get_id();


	friend class Scene;
};


class Light {
private:
	// TODO - light position
	// TODO - other light parameters
public:

	friend class Scene;
};


class Camera {
private:
	const int m_cam_id;
	const Point3D m_position;
	const Vector3D m_view;
	const double m_fov;
	Matrix4x4 m_projection;

	Camera(CameraNode *cam);
public:
	virtual ~Camera();

	Matrix4x4 px_to_wcs(const int w, const int h);
	
	int get_id();

	friend class Scene;
};




class Scene {
private:
	std::vector<SceneObject*> m_objects;
	std::vector<Light*> m_lights;
	std::vector<Camera*> m_cameras;

	static void make_scene(SceneNode *node,
			const Matrix4x4 &trans,
			Scene *scene);
public:
	Scene();
	virtual ~Scene();

	Camera *get_camera(const int cam_id);

	static void make_scene(SceneNode *node,
			Scene *scene);
};

#endif
