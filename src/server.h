#ifndef SERVER_H
#define SERVER_H

#include <facebase.h>

class Server
{
  public:
    Server(int port, FaceBase*);
    int start(void);
    int stop(void);
    std::mutex facebase_mutex;
    FaceBase *facebase;
    int port;
  private:
    pthread_t server_thread;
};

#endif // SERVER_H
