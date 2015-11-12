#ifndef FACE_H
#define FACE_H

#include <photo.h>

class Face
{
  public:
    Face(std::string /*face_id*/, std::string /*node_id*/, Photo*);
    std::string id;
  private:
    std::string created_time;
    std::string node_id;
    Photo *photo;
    float x,y;
};

#endif // FACE_H
