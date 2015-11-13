#ifndef NODE_H
#define NODE_H

#include <list>

class Node;

#include <face.h>
#include <photo.h>

class Node
{
  public:
    Node(std::string /*id*/);
    std::string id;
    void addFace(Face* face);
    std::list<Face*> faces;
  private:
};

#endif // NODE_H
