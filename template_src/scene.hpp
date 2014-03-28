#ifndef __SCENE_H__
#define __SCENE_H__

#include <vector>

#include "algebra.hpp"
#include "material.hpp"
#include "primitive.hpp"
#include "scene_tree.hpp"


class Ray;
class Intersection;
class Scene;

class CsgObject;

class SceneObject {
protected:
	const int m_id;
	const Matrix4x4 m_trans;
	const Matrix4x4 m_itrans;
	
	SceneObject(const int id,
			const Matrix4x4 &trans);

	virtual void intersection(const Ray *ray,
			std::vector<IntersectionData> &intersections) = 0;

public:
	virtual ~SceneObject();

	int get_id();

	void intersection(const Ray *ray, Intersection *i);

	friend class CsgObject;
};

class PrimitiveObject : public SceneObject {
private:
	Primitive *const m_prim;
	Material *const m_material;
protected:
	PrimitiveObject(const int id,
			const Matrix4x4 &trans,
			Primitive *primitive,
			Material *material);

	virtual void intersection(const Ray *ray,
			std::vector<IntersectionData> &intersections);
public:
	virtual ~PrimitiveObject();

	Primitive *get_primitive()
	{
		return m_prim;
	}

	Material *get_material()
	{
		return m_material;
	}

	friend class Scene;
};

class CsgObject : public SceneObject {
private:
	CsgOp m_op;
	SceneObject *const m_left;
	SceneObject *const m_right;
protected:
	CsgObject(const int id,
			const Matrix4x4 &trans,
			CsgOp op,
			SceneObject *left,
			SceneObject *right);

	virtual void intersection(const Ray *ray,
			std::vector<IntersectionData> &intersections);
public:
	virtual ~CsgObject();

	friend class Scene;
};


class Light {
private:
	// TODO - light position
	// TODO - other light parameters
protected:
	Light(LightNode *node);
public:
	const Colour color;
	const Point3D position;
	double falloff[3];

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

	const Point3D &get_pos() const;
	
	int get_id();

	friend class Scene;
};

class IntersectionStrategy;

class Scene {
private:
	std::vector<SceneObject*> m_objects;
	std::vector<Light*> m_lights;
	std::vector<Camera*> m_cameras;

	static SceneObject *make_scene_object(NodeType type,
			SceneNode *node,
			const Matrix4x4 &trans,
			const int id);

	static void make_scene(SceneNode *node,
			const Matrix4x4 &trans,
			Scene *scene);
public:
	Scene();
	virtual ~Scene();

	void fill(IntersectionStrategy *is);

	Camera *get_camera(const int cam_id);

	int get_light_count();
	Light *get_light(int i);

	static void make_scene(SceneNode *node,
			Scene *scene);
};

#endif
