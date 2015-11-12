#ifndef SERVER_H
#define SERVER_H

#include <facebase.h>

class Server
{
  public:
    Server(int port, FaceBase*);
    int run(void);
  private:
    int port;
};

#endif // SERVER_H
