#include <node.h>

Node::Node(std::string id)
{
  this->id.assign(id);
}

void Node::addFace(Face* face)
{
  //faces[face->id] = face;

  faces_mutex.lock();
  faces.push_front(face);
  faces_mutex.unlock();
}
