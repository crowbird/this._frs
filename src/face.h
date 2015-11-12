#ifndef FACE_H
#define FACE_H

#include <iostream>

class Face
{
  public:
    Face(int /*face_idx*/, std::string /*node_id*/, double x, double y, std::string /*face_name*/);
    double x,y;
    std::string name;
  private:
    std::string created_time;
    std::string node_id;
};

#endif // FACE_H
