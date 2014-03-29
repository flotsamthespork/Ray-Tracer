
#include <algorithm>
#include <float.h>
#include <iostream>
#include <set>
#include "ray.hpp"
#include "intersection.hpp"

#include "scene.hpp"


SceneObject::SceneObject(const int id,
		const Matrix4x4 *trans) :
	m_id(id),
	m_trans(trans ? new Matrix4x4(*trans) : NULL),
	m_itrans(trans ? new Matrix4x4(trans->invert()) : NULL)
{
}

SceneObject::~SceneObject()
{
	if (m_trans)
		delete m_trans;
	if (m_itrans)
		delete m_itrans;
}


int
SceneObject::get_id()
{
	return m_id;
}

void
SceneObject::complete_intersection(IntersectionData &d)
{
	if (m_trans)
	{
		Matrix4x4 t = m_itrans->transpose();
		d.normal = t * d.normal;
		d.u_tangent = t * d.u_tangent;
	}
}

void
SceneObject::intersection(const Ray *ray,
		Intersection *i,
		std::vector<IntersectionData> &intersections)
{
	Ray new_ray;
	const Ray *r;
	double t_multiplier;
	if (m_trans)
	{
		ray->transform(m_itrans, new_ray);
		r = &new_ray;
		t_multiplier = 1/new_ray.get_dir().length();
		new_ray.normalize();
	}
	else
	{
		r = ray;
		t_multiplier = 1.0;
	}

	IntersectionHelper helper(this, i, &intersections, t_multiplier);
	intersection(r, &helper);
}

/*
void
SceneObject::intersection(const Ray *ray, Intersection *i)
{
	i->m_vec->resize(0);
	intersection(ray, *i->m_vec);

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
*/
PrimitiveObject::PrimitiveObject(const int id,
			const Matrix4x4 *trans,
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
		IntersectionHelper *helper)
{
	m_prim->intersection(ray, helper);
}

void
PrimitiveObject::complete_intersection(IntersectionData &d)
{
	SceneObject::complete_intersection(d);
	d.object = this;
}

CsgObject::CsgObject(const int id,
			const Matrix4x4 *trans,
			CsgOp op,
			SceneObject *left,
			SceneObject *right) :
	SceneObject(id, trans),
	m_op(op),
	m_left(left),
	m_right(right)
{
}

CsgObject::~CsgObject()
{
	if (m_left)
		delete m_left;
	if (m_right)
		delete m_right;
}

void
CsgObject::intersection(const Ray *ray,
		IntersectionHelper *helper)
{
	std::vector<IntersectionData> left_i;
	std::vector<IntersectionData> right_i;

	if (m_left)
		m_left->intersection(ray, helper->get_intersection(), left_i);
	if (m_right)
		m_right->intersection(ray, helper->get_intersection(), right_i);

	std::sort(left_i.begin(), left_i.end());
	std::sort(right_i.begin(), right_i.end());

	std::set<PrimitiveObject*> in_l;
	std::set<PrimitiveObject*> in_r;

	int li = 0, ri = 0;
	const int ls = left_i.size(),
	      rs = right_i.size();

	while (li < ls || ri < rs)
	{
		const double lt = (li < ls ? left_i[li].t : DBL_MAX);
		const double rt = (ri < rs ? right_i[ri].t : DBL_MAX);

		const bool l = (lt < rt);

		if (l)
		{
			PrimitiveObject *o = left_i[li].object;
			std::set<PrimitiveObject*>::iterator it = in_l.find(o);
			if (it != in_l.end())	in_l.erase(o);
			else			in_l.insert(o);
		}
		else
		{
			PrimitiveObject *o = right_i[ri].object;
			std::set<PrimitiveObject*>::iterator it = in_r.find(o);
			if (it != in_r.end())	in_r.erase(o);
			else			in_r.insert(o);
		}

		switch (m_op)
		{
		case UNION:
			if (l)
				helper->on_intersection(left_i[li]);
			else
				helper->on_intersection(right_i[ri]);
			break;
		case INTERSECTION:
			if (l && in_r.size() > 0)
				helper->on_intersection(left_i[li]);
			else if (!l && in_l.size() > 0)
				helper->on_intersection(right_i[ri]);
			break;
		case DIFFERENCE:
			if (l && in_r.size() == 0)
				helper->on_intersection(left_i[li]);
			else if (!l && in_l.size() > 0 && in_r.size() == 0)
			{
				right_i[ri].normal = -1*right_i[ri].normal;
				helper->on_intersection(right_i[ri]);
			}
		}

		if (l)
			li++;
		else
			ri++;
	}
}

IntersectionCache*
IntersectionHelper::get_cache(int size)
{
	IntersectionCache *c = m_intersection->get_cache();
	if (c)
		return c->get_subcache(m_obj->get_id(), size);
	return 0;
}


Light::Light(LightNode *node, const Matrix4x4 &trans) :
	color(node->color),
	position(trans*node->position)
{
	falloff[0] = node->falloff[0];
	falloff[1] = node->falloff[1];
	falloff[2] = node->falloff[2];
}


Camera::Camera(CameraNode *cam, const Matrix4x4 &trans) :
	m_cam_id(cam->cam_id),
	m_position(!cam->rel_pos ? cam->position :
			(trans * cam->position)),
	m_view(!cam->rel_view ? cam->view :
			(trans * cam->view)),
	m_fov(cam->fov)
{
	Vector3D up = (!cam->rel_up ? cam->up :
			(trans * cam->up));
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


SceneObject*
Scene::make_scene_object(NodeType type,
		SceneNode *node,
		const Matrix4x4 &trans,
		const int id)
{
	SceneObject *obj = 0;
	if (type == GEOMETRY)
	{
		GeometryNode *g = (GeometryNode*) node;
		Material *m = g->material;
		Primitive *p = g->primitive;
		if (m && p)
			obj = new PrimitiveObject(id, &trans, p, m);
	}
	else if (type == CSG)
	{
		CsgNode *c = (CsgNode*) node;
		SceneObject *l = make_scene_object(c->m_left->get_type(),
				c->m_left, c->m_left->get_transform(), -1);
		SceneObject *r = make_scene_object(c->m_right->get_type(),
				c->m_right, c->m_right->get_transform(), -1);

		obj = new CsgObject(id, &trans, c->m_op, l, r);
	}

	return obj;
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
	case CSG:
	case GEOMETRY:
	{
		SceneObject *o = make_scene_object(node->get_type(), node,
				ntrans, scene->m_objects.size());
		if (o)
			scene->m_objects.push_back(o);
	}
		break;
	case CAMERA:
	{
		CameraNode *c = (CameraNode*) node;
		scene->m_cameras.push_back(new Camera(c,trans));
	}
		break;
	case LIGHT:
	{
		LightNode *l = (LightNode*) node;
		scene->m_lights.push_back(new Light(l,trans));
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

