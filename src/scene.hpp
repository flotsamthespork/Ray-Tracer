#ifndef __SCENE_H__
#define __SCENE_H__

#include <vector>

#include "algebra.hpp"
#include "material.hpp"
#include "primitive.hpp"
#include "scene_tree.hpp"
#include "bounds.hpp"


class Ray;
class Intersection;
class IntersectionCache;
class IntersectionStrategy;
class Scene;

class CsgObject;
class IntersectionHelper;

class SceneObject {
protected:
	const int m_id;
	const Matrix4x4 *m_trans;
	const Matrix4x4 *m_itrans;
	
	SceneObject(const int id,
			const Matrix4x4 *trans);

	virtual void intersection(const Ray *ray,
			IntersectionHelper *helper) = 0;
public:
	virtual ~SceneObject();

	int get_id();

	void intersection(const Ray *ray,
			Intersection *intersection,
			std::vector<IntersectionData> &intersections);

	virtual void get_bounds(Bounds &bounds) = 0;

	virtual void complete_intersection(IntersectionData &d);

	virtual void finish(IntersectionStrategy *is)
	{
		(void)is;
	}

	friend class CsgObject;
};


class PrimitiveObject : public SceneObject {
private:
	Primitive *const m_prim;
	Material *const m_material;
protected:
	virtual void intersection(const Ray *ray,
			IntersectionHelper *helper);
public:
	PrimitiveObject(const int id,
			const Matrix4x4 *trans,
			Primitive *primitive,
			Material *material);

	virtual ~PrimitiveObject();

	virtual void complete_intersection(IntersectionData &d);

	Primitive *get_primitive()
	{
		return m_prim;
	}

	Material *get_material()
	{
		return m_material;
	}

	virtual void get_bounds(Bounds &bounds);

	virtual void finish(IntersectionStrategy *is)
	{
		m_prim->finish(is);
	}
};

class CsgObject : public SceneObject {
private:
	CsgOp m_op;
	SceneObject *const m_left;
	SceneObject *const m_right;
protected:
	CsgObject(const int id,
			const Matrix4x4 *trans,
			CsgOp op,
			SceneObject *left,
			SceneObject *right);

	virtual void intersection(const Ray *ray,
			IntersectionHelper *helper);
public:
	virtual ~CsgObject();

	virtual void get_bounds(Bounds &bounds);

	friend class Scene;
};

class IntersectionHelper {
private:
	std::vector<IntersectionData>*	m_intersections;
	SceneObject*			m_obj;
	Intersection*			m_intersection;
	double				m_t_multiplier;
public:
	IntersectionHelper(SceneObject *obj,
			Intersection *intersection,
			std::vector<IntersectionData>* intersections,
			double t_multiplier) :
		m_intersections(intersections),
		m_obj(obj),
		m_intersection(intersection),
		m_t_multiplier(t_multiplier)
	{
	}

	Intersection *get_intersection()
	{
		return m_intersection;
	}

	IntersectionCache *get_cache(int size);

	void on_intersection(IntersectionData &d)
	{
		d.t *= m_t_multiplier;
		m_obj->complete_intersection(d);
		m_intersections->push_back(d);
	}
	
};

class Light {
private:
	// TODO - light position
	// TODO - other light parameters
protected:
	Light(LightNode *node, const Matrix4x4 &trans);
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

	Camera(CameraNode *cam, const Matrix4x4 &trans);
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
