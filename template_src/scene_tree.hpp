#ifndef __SCENE_TREE_HPP__
#define __SCENE_TREE_HPP__

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"

enum NodeType {
	NODE,
	JOINT,
	GEOMETRY,
	LIGHT,
	CAMERA
};

class SceneNode {
public:
	SceneNode(const std::string& name);
	virtual ~SceneNode();

	const Matrix4x4& get_transform() const { return m_trans; }
	const Matrix4x4& get_inverse() const { return m_invtrans; }
	
	void set_transform(const Matrix4x4& m)
	{
		m_trans = m;
		m_invtrans = m.invert();
	}

	void set_transform(const Matrix4x4& m, const Matrix4x4& i)
	{
		m_trans = m;
		m_invtrans = i;
	}

	void add_child(SceneNode* child)
	{
		m_children.push_back(child);
	}

	void remove_child(SceneNode* child)
	{
		m_children.remove(child);
	}

	// Callbacks to be implemented.
	// These will be called from Lua.
	void rotate(char axis, double angle);
	void scale(const Vector3D& amount);
	void translate(const Vector3D& amount);

	virtual NodeType get_type() const;

	std::list<SceneNode*> *get_children()
	{
		return &m_children;
	}
	
protected:
	
	// Useful for picking
	std::string m_name;

	// Transformations
	Matrix4x4 m_trans;
	Matrix4x4 m_invtrans;

	// Hierarchy
	typedef std::list<SceneNode*> ChildList;
	ChildList m_children;
};

class JointNode : public SceneNode {
public:
	JointNode(const std::string& name);
	virtual ~JointNode();

	virtual NodeType get_type() const;

	void set_joint_x(double min, double init, double max);
	void set_joint_y(double min, double init, double max);

	struct JointRange {
		double min, init, max;
	};

	
protected:

	JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
	Material* material;
	Primitive* primitive;

	GeometryNode(const std::string& name,
		           Primitive* primitive);
	virtual ~GeometryNode();

	virtual NodeType get_type() const;
};


class LightNode : public SceneNode {
public:
	Colour color;
	Point3D position;
	double falloff[3];

	LightNode(const std::string &name);
	virtual ~LightNode();

	virtual NodeType get_type() const;
};


class CameraNode : public SceneNode {
public:
	const int	cam_id;
	Point3D		position;
	Vector3D	up;
	Vector3D	view;
	double		fov;

	bool rel_pos;
	bool rel_up;
	bool rel_view;

	CameraNode(const std::string &name, const int cam_id);
	virtual ~CameraNode();

	virtual NodeType get_type() const;
};

#endif
