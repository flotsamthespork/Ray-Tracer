
#include <algorithm>
#include <iostream>
#include "ray.hpp"
#include "intersection.hpp"

#include "scene.hpp"


SceneObject::SceneObject(const int id,
		const Matrix4x4 &trans) :
	m_id(id),
	m_trans(trans),
	m_itrans(trans.invert())
{
}

SceneObject::~SceneObject()
{
}


int
SceneObject::get_id()
{
	return m_id;
}

void
SceneObject::intersection(const Ray *ray, Intersection *i)
{
//	std::vector<IntersectionData> intersections;
	i->m_vec->resize(0);
	intersection(ray, *i->m_vec);
	// TODO

	std::sort(i->m_vec->begin(), i->m_vec->end());

	if (i->m_vec->size() > 0)
	{
		for (std::vector<IntersectionData>::iterator it = i->m_vec->begin();
				it != i->m_vec->end(); ++it)
		{
			if (it->t > i->m_eps)
			{
				i->update_intersection(*it);
				break;
			}
		}
	}
}

PrimitiveObject::PrimitiveObject(const int id,
			const Matrix4x4 &trans,
			Primitive *primitive,
			Material *material) :
	SceneObject(id, trans),
	m_prim(primitive),
	m_material(material)
{
}

PrimitiveObject::~PrimitiveObject()
{
}

void
PrimitiveObject::intersection(const Ray *ray,
		std::vector<IntersectionData> &intersections)
{
	Ray new_ray;
	ray->transform(m_itrans, new_ray);

	m_prim->intersection(&new_ray, intersections);

	for (std::vector<IntersectionData>::iterator i = intersections.begin();
			i != intersections.end(); ++i)
	{
		i->object = this;
		i->normal = m_itrans.transpose() * i->normal;
		i->u_tangent = m_itrans.transpose() * i->normal;
	}
}

CsgObject::CsgObject(const int id,
			const Matrix4x4 &trans,
			SceneObject *left,
			SceneObject *right) :
	SceneObject(id, trans),
	m_left(left),
	m_right(right)
{
}

CsgObject::~CsgObject()
{
}

void
CsgObject::intersection(const Ray *ray,
		std::vector<IntersectionData> &intersections)
{
	Ray new_ray;
	ray->transform(m_itrans, new_ray);

	std::vector<IntersectionData> left_i;
	std::vector<IntersectionData> right_i;

	m_left->intersection(&new_ray, left_i);
	m_right->intersection(&new_ray, right_i);

	// TODO - merge based on the CSG operator

	for (std::vector<IntersectionData>::iterator i = intersections.begin();
			i != intersections.end(); ++i)
	{
		i->normal = m_itrans.transpose() * i->normal;
		i->u_tangent = m_itrans.transpose() * i->normal;
	}
}

Light::Light(LightNode *node) :
	color(node->color),
	position(node->get_transform()*node->position)
{
	falloff[0] = node->falloff[0];
	falloff[1] = node->falloff[1];
	falloff[2] = node->falloff[2];
}


Camera::Camera(CameraNode *cam) :
	m_cam_id(cam->cam_id),
	m_position(!cam->rel_pos ? cam->position :
			(cam->get_transform() * cam->position)),
	m_view(!cam->rel_view ? cam->view :
			(cam->get_transform() * cam->view)),
	m_fov(cam->fov)
{
	Vector3D up = (!cam->rel_up ? cam->up :
			(cam->get_transform() * cam->up));
	Vector3D eye_vec(m_position[0],
			m_position[1],
			m_position[2]);
	Vector3D w = m_view;		w.normalize();
	Vector3D u = up.cross(w);	u.normalize();
	Vector3D v = w.cross(u);	v.normalize();

	m_projection = translation(eye_vec) * Matrix4x4(u, v, w);
}

Camera::~Camera()
{
}

Matrix4x4
Camera::px_to_wcs(const int w, const int h)
{
	const double d = m_view.length();
	const double hd = 2 * d * tan(M_PI * m_fov / 360.0);
	const double wd = hd * (double)w / (double)h;
	return m_projection *
		scaling(Vector3D(-wd/w, -hd/h, 1)) *
		translation(Vector3D(-w/2, -h/2, d));
}

const Point3D&
Camera::get_pos() const
{
	return m_position;
}

int
Camera::get_id()
{
	return m_cam_id;
}

Scene::Scene()
{
}


Scene::~Scene()
{
	for (std::vector<SceneObject*>::iterator i = m_objects.begin();
			i != m_objects.end(); ++i)
		delete *i;
	for (std::vector<Camera*>::iterator i = m_cameras.begin();
			i != m_cameras.end(); ++i)
		delete *i;
	for (std::vector<Light*>::iterator i = m_lights.begin();
			i != m_lights.end(); ++i)
		delete *i;
}


Camera*
Scene::get_camera(const int cam_id)
{
	Camera *c = 0;
	for (std::vector<Camera*>::iterator i = m_cameras.begin();
			i != m_cameras.end(); ++i)
	{
		if ((*i)->get_id() == cam_id)
		{
			c = *i;
			break;
		}
	}
	return c;
}

void
Scene::fill(IntersectionStrategy *is)
{
	for (std::vector<SceneObject*>::iterator i = m_objects.begin();
			i != m_objects.end(); ++i)
	{
		is->add_object(*i);
	}
}

int
Scene::get_light_count()
{
	return m_lights.size();
}

Light*
Scene::get_light(int i)
{
	return m_lights[i];
}

void
Scene::make_scene(SceneNode *node,
		const Matrix4x4 &trans,
		Scene *scene)
{
	if (!node)
		return;

	const Matrix4x4 ntrans = trans * node->get_transform();

	switch (node->get_type())
	{
	case GEOMETRY:
	{
		GeometryNode *g = (GeometryNode*) node;
		Material *m = g->material;
		Primitive *p = g->primitive;
		if (m && p)
			scene->m_objects.push_back(new PrimitiveObject(
						scene->m_objects.size(),
						ntrans, p, m));
	}
		break;
	case CAMERA:
	{
		CameraNode *c = (CameraNode*) node;
		scene->m_cameras.push_back(new Camera(c));
	}
		break;
	case LIGHT:
	{
		LightNode *l = (LightNode*) node;
		scene->m_lights.push_back(new Light(l));
	}
		break;
	default: break;
	}

	std::list<SceneNode*> *l = node->get_children();

	for (std::list<SceneNode*>::iterator i = l->begin();
			i != l->end(); ++i)
		make_scene(*i, ntrans, scene);
}


void
Scene::make_scene(SceneNode *node, Scene *scene)
{
	Matrix4x4 id_mtx;
	make_scene(node, id_mtx, scene);
}

