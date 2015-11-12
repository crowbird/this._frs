#include <node.h>

Node::Node(std::string id)
{
  this->id.assign(id);
}

Node::addFace(Face* face)
{
  faces[face->id] = face;
}
