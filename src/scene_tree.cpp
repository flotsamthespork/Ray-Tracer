#include "scene_tree.hpp"
#include <iostream>

SceneNode::SceneNode(const std::string& name)
	: m_name(name)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::rotate(char axis, double angle)
{
	set_transform(m_trans * rotation(angle, axis));
}

void SceneNode::scale(const Vector3D& amount)
{
	set_transform(m_trans * scaling(amount));
}

void SceneNode::translate(const Vector3D& amount)
{
	set_transform(m_trans * translation(amount));
}

NodeType SceneNode::get_type() const
{
	return NODE;
}


JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
}

JointNode::~JointNode()
{
}

NodeType JointNode::get_type() const
{
	return JOINT;
}

void JointNode::set_joint_x(double min, double init, double max)
{
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

void JointNode::set_joint_y(double min, double init, double max)
{
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
	: SceneNode(name),
	  primitive(primitive)
{
}

GeometryNode::~GeometryNode()
{
}

NodeType GeometryNode::get_type() const
{
	return GEOMETRY;
}


LightNode::LightNode(const std::string &name) :
	SceneNode(name),
	color(0)
{
	falloff[0] = 1;
	falloff[1] = 0;
	falloff[2] = 0;
}

LightNode::~LightNode()
{
}

NodeType
LightNode::get_type() const
{
	return LIGHT;
}


CameraNode::CameraNode(const std::string &name,
		const int cam_id) :
	SceneNode(name),
	cam_id(cam_id),
	up(0,1,0),
	view(0,0,1),
	fov(45),
	rel_pos(false),
	rel_up(false),
	rel_view(false)
{
}

CameraNode::~CameraNode()
{
}

NodeType
CameraNode::get_type() const
{
	return CAMERA;
}
