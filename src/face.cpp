#include <stdio.h>

#include <face.h>

Face::Face(int face_idx, std::string node_id, std::string photo_id, double tag_x, double tag_y, std::string face_name)
{
  this->detect = false;
  this->node_id.assign(node_id);
  this->photo_id.assign(photo_id);
  this->name.assign(face_name);
  this->tag_x = tag_x;
  this->tag_y = tag_y;
}

void Face::markDetect(double detect_x, double detect_y, double distance, int idx)
{
  this->detect_x = detect_x;
  this->detect_y = detect_y;
  this->distance = distance;
  this->face_detect_idx = idx;
  this->detect = true;
}

