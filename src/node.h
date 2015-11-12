#ifndef NODE_H
#define NODE_H

#include <map>

#include <face.h>
#include <photo.h>

class Node
{
  public:
    Node(std::string /*id*/);
    std::string id;
    void addFace(Face* face);
  private:
    std::map<std::string,Face*> faces;
};

#endif // NODE_H
