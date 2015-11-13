#ifndef FACEBASE_H
#define FACEBASE_H

#include <iostream>
#include <map>
#include <queue>

#include <photo.h>
#include <face.h>
#include <node.h>

#define DISTANCE_THRESHOLD 50

class FaceBase
{
  public:
    FaceBase();
    void detectFaces(std::string /*photo_id*/);
    void newPhoto(std::string /*photo_id*/, std::string /*link*/, int tag_count);
    Face* newFace(int /*face_idx*/, std::string /*photo_id*/, std::string /*node_id*/, double x, double y, std::string face_name);
    std::queue<Photo*> valid_photos;
    Photo* getNewPhoto(void);
    std::map<std::string, Node*> nodes;
    std::map<std::string, Photo*> photos;
  private:
    //Node* findNode(std::string /*node_id*/);
    Node* newNode(std::string /*node_id*/);
    //std::map<std::string, Face*> faces;
};

#endif // FACEBASE_H
