#ifndef PHOTO_H
#define PHOTO_H

#include <iostream>
#include <cstdint>
#include <unistd.h>

#define PHOTOBASE_ROOT "/home/cgreen/photobase"

class Photo
{
  public:
    Photo(std::string /*photo_id*/, std::string /*link*/);
    int downloadLink(void);
    std::string file_path;
  private:
    bool downloaded(void);
    std::string id;
    std::string link;
};

#endif /* PHOTO_H */
