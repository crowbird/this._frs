#include <pthread.h>
#include <QString>
#include <iostream>

// OpenBR/OpenCV
#include <openbr/openbr_plugin.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

// frs
#include <server.h>
#include <eye.h>
#include <facebase.h>

int main(int argc, char *argv[])
{
  int ret;

  br::Context::initialize(argc, argv);
  br_set_property("enrollALL","true");
  br::Globals->enrollAll = true;

  // start database
  FaceBase* facebase = new FaceBase();
  facebase->start();

  // start web server
  Server* server = new Server(55555, facebase);
  server->start();

  // watch for faces
  Eye* eye = new Eye(facebase);
  eye->run();
  
  server->stop();
  facebase->stop();

  br::Context::finalize();
  return 0;
}
