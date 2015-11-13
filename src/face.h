#ifndef FACE_H
#define FACE_H

#include <iostream>
#include <openbr/openbr_plugin.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

class Face
{
  public:
    Face(int /*face_idx*/, std::string /*node_id*/, std::string /*photo_id*/, double x, double y, std::string /*face_name*/);
    void markDetect(double /*detect_x*/, double /*detect_y*/, double /*distance*/, int idx);
    double tag_x,tag_y;
    double detect_x, detect_y, distance;
    bool detect;
    int face_detect_idx;
    std::string name;
    std::string node_id;
    std::string photo_id;
    br::Template face_template;
    int match_count;
  private:
    std::string created_time;
};

#endif // FACE_H
