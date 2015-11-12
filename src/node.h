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
  private:
    std::map<int,Face*> faces;
};

#endif // NODE_H
