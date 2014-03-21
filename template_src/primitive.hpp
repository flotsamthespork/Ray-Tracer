#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"

class Primitive {
public:
  virtual ~Primitive() {}
};


class Sphere : public Primitive {
public:
  Sphere(const Point3D& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~Sphere()
  {}

private:
  Point3D m_pos;
  double m_radius;
};

class Box : public Primitive {
public:
  Box(const Point3D& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~Box()
  {}

private:
  Point3D m_pos;
  double m_size;
};

#endif
