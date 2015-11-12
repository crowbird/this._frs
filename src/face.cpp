#include <stdio.h>

#include <face.h>

Face::Face(int face_idx, std::string node_id, double x, double y, std::string face_name)
{
  this->node_id.assign(node_id);
  this->name.assign(face_name);
  this->x = x;
  this->y = y;
}
