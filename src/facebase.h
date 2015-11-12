#include <iostream>
#include <map>

#include <photo.h>
#include <face.h>
#include <node.h>

class FaceBase
{
  public:
    FaceBase();
    void newPhoto(std::string /*photo_id*/, std::string /*link*/);
    Face* newFace(std::string /*face_id*/, std::string /*photo_id*/, std::string /*node_id*/, float x, float y);
  private:
    //Node* findNode(std::string /*node_id*/);
    Node* newNode(std::string /*node_id*/);
    std::map<std::string, Photo*> photos;
    std::map<std::string, Node*> nodes;
    std::map<std::string, Face*> faces;
};
