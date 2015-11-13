#ifndef PHOTO_H
#define PHOTO_H

#include <iostream>
#include <list>
#include <cstdint>
#include <unistd.h>

class Photo;

#include <face.h>

#define PHOTOBASE_ROOT "/home/cgreen/photobase"

class Photo
{
  public:
    Photo(std::string /*photo_id*/, std::string /*link*/, int /*face_count*/);
    int downloadLink(void);
    std::string file_path;
    Face** faces;
    int face_count;
    bool valid;
    br::TemplateList face_templates;
  private:
    bool downloaded(void);
    std::string id;
    std::string link;
};

#endif /* PHOTO_H */
