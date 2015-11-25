#ifndef NODE_H
#define NODE_H

#include <list>
#include <mutex>

class Node;

#include <face.h>
#include <photo.h>

class Node
{
  public:
    Node(std::string /*id*/);
    std::string id;
    void addFace(Face* face);
    std::mutex faces_mutex;
    std::list<Face*> faces;
  private:
};

#endif // NODE_H
