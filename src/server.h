#include <photo.h>

class Server
{
  public:
    Server(int port, void (*RXPhotoCallback)(Photo*));
    int start(void);
  private:
    int port;
    void (*RXPhotoCallback)(Photo*);
};
