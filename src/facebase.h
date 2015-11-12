#include <iostream>

#include <photo.h>
#include <face.h>
#include <node.h>

class FaceBase
{
  public:
    FaceBase();
    void newPhoto(std::string /*photo_id*/, std::string /*link*/);
    //newFace(std::string /*face_id*/, float x, float y);
  private:
    //Node* findNode(std::string /*node_id*/);
    //Node* newNode(std::string /*node_id*/);
    //std::map<std::string, Photo*> photos;
    //std::map<std::string, Node*> nodes;
    //std::map<std::string, Face*> faces;
};
