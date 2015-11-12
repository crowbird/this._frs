#include <node.h>

Node::Node(std::string id)
{
  this->id.assign(id);
}

void Node::addFace(Face* face)
{
  //faces[face->id] = face;
  faces.push_front(face);
}
