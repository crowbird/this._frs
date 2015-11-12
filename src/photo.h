#ifndef PHOTO_H
#define PHOTO_H

#include <iostream>
#include <cstdint>

#define PHOTOBASE_ROOT "/home/cgreen/photobase"

class Photo
{
  public:
    Photo(std::string /*photo_id*/, std::string /*link*/);
    std::string getFilePath(void);
  private:
    std::string id;
    std::string link;
    int downloadLink(void);
};

#endif /* PHOTO_H */
