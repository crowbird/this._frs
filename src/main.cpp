#include <pthread.h>
#include <QString>
#include <iostream>
#include <openbr/openbr_plugin.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <server.h>
#include <eye.h>
#include <facebase.h>

static void printTemplate(const br::Template &t)
{
    const QPoint firstEye = t.file.get<QPoint>("Affine_0");
    const QPoint secondEye = t.file.get<QPoint>("Affine_1");
    printf("%s eyes: (%d, %d) (%d, %d)\n", qPrintable(t.file.fileName()), firstEye.x(), firstEye.y(), secondEye.x(), secondEye.y());
}

void newPhoto(Photo* photo)
{
}

FaceBase facebase;

int visualize_photo()
{
  cv::Mat img = cv::imread("/home/cgreen/photobase/test", CV_LOAD_IMAGE_UNCHANGED);

  cv::namedWindow("MyWindow", CV_WINDOW_AUTOSIZE); //create a window with the name "MyWindow"
  cv::imshow("MyWindow", img); //display the image which is stored in the 'img' in the "MyWindow" window

  cv::waitKey(5000); //wait infinite time for a keypress

  cv::destroyWindow("MyWindow"); //destroy the window with the name, "MyWindow"
  return 0;
}

void* server_function(void *context)
{
  Server *server = (Server*)context;
  server->run();
  return server;
}

void* eye_function(void* context)
{
  Eye *eye = (Eye*)context;
  eye->run();
  return eye;
}

int main(int argc, char *argv[])
{
  int ret;

  br::Context::initialize(argc, argv);
  br_set_property("enrollALL","true");
  br::Globals->enrollAll = true;

  pthread_t server_thread;

  Server server = Server(55555, &facebase);
  ret = pthread_create(&server_thread, NULL, server_function, &server); 
  if (ret < 0)
  {
    return ret;
  }

  Eye eye = Eye(&facebase);
  eye_function(&eye);
  
  pthread_join(server_thread,NULL);

  br::Context::finalize();
  return 0;
}
//! [face_recognition]
