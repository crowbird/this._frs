#include <stdio.h>

#include <face.h>

Face::Face(std::string face_id, std::string node_id, Photo *photo, float x, float y)
{
  id.assign(face_id);
  node_id.assign(node_id);
  this->photo = photo;
  this->x = x;
  this->y = y;
}
