
#include "scene.hpp"



SceneObject::SceneObject(const int id,
		const Matrix4x4 &trans,
		Primitive *primitive,
		Material *material) :
	m_id(id),
	m_trans(trans),
	m_itrans(trans.invert()),
	m_prim(primitive),
	m_material(material)
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
	Vector3D eye_vec(cam->position[0],
			cam->position[1],
			cam->position[2]);
	Vector3D w = cam->view;		w.normalize();
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
			scene->m_objects.push_back(new SceneObject(
						scene->m_objects.size(),
						ntrans, p, m));
	}
		break;
	case CAMERA:
	{
		CameraNode *c = (CameraNode*) node;
		scene->m_cameras.push_back(new Camera(c));
	}
	case LIGHT:
	{
		// TODO
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

