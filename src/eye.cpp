#include <QString>
#include <iostream>
#include <limits>
#include <openbr/openbr_plugin.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <facebase.h>
#include <eye.h>

Eye::Eye(FaceBase* fb)
{
  eye_facebase = fb;
}

void Eye::status(void)
{
  printf("photos:%d nodes:%d faces:%d\n", eye_facebase->photo_count, eye_facebase->node_count, eye_facebase->face_count);
}

void Eye::showFace(Face* face)
{
  Photo* photo = eye_facebase->photos[face->photo_id];
  try
  {
    if (!photo->downloaded)
      return;
    cv::Mat vis;
    vis = cv::imread(photo->file_path.c_str(), CV_LOAD_IMAGE_UNCHANGED);

    cv::Point fe(face->detect_x, face->detect_y), se(face->tag_x, face->tag_y);
    cv::circle(vis, fe, 50.0, cv::Scalar(255, 0, 0), 3, 3);
    cv::circle(vis, se, 50.0, cv::Scalar(0, 0, 255), 3, 3);

    std::ostringstream report1;
    std::ostringstream report2;
    report1 << "name " << face->name << " matches " << std::to_string(face->match_count);

    putText(vis, report1.str().c_str(), cv::Point(10, 30),
                   cv::FONT_HERSHEY_PLAIN, 2,
                   cv::Scalar(0, 0, 255),
                   2, 3);

    report2 << "photos " << std::to_string(eye_facebase->photo_count) 
            << " faces " << std::to_string(eye_facebase->face_count)
            << " nodes " << std::to_string(eye_facebase->node_count);

    putText(vis, report2.str().c_str(), cv::Point(10, 90),
                   cv::FONT_HERSHEY_PLAIN, 2,
                   cv::Scalar(0, 0, 255),
                   2, 3);

    cv::imshow("TheEye", vis);
  }
  catch (std::exception e)
  {
    std::cout << "showFace exception";
  }
}

void Eye::viewNewPhotos(void)
{
  printf("View New Photos Press 'q' to exit.\n");
  Photo* photo;
  while(photo = eye_facebase->nextValidPhoto())
  {
    for (int face_idx = 0; face_idx < photo->face_count; face_idx++)
    {
      Face* face = photo->faces[face_idx];
      if (face->detect)
      {
	showFace(face);
        int c = cv::waitKey();
	printf("c:%d\n", c);
        if ((c == 'c') || (c == 'q') || (c == 27)) {
            return;
        }
      }
    }
  }
}

void Eye::cycleAllPhotos(void)
{
  printf("View New Photos Press 'q' to exit.\n");
  cv::Mat vis;
  Photo* photo;
  bool running = false;
  while (1)
  {
    for (auto& photo_pair : eye_facebase->photos)
    {
      photo = photo_pair.second;
      //printf("show photo %p %d\n", photo, (photo == NULL ? 0 : photo->valid));
      if (photo != NULL && photo->valid)
      {
        for (int face_idx = 0; face_idx < photo->face_count; face_idx++)
        {
          Face* face = photo->faces[face_idx];
          //printf("show face %p %d\n", face, (face == NULL ? 0 : face->detect));
          if (face != NULL && face->detect)
          {
            showFace(face);
            int c = cv::waitKey(500);
	    //printf("c:%d\n", c);
            if ((c == 'c') || (c == 'q') || (c == 27))
	    {
              return;
            }
	    if ((c == 114))
	    {
	      printf("searching for matches\n");
              eye_facebase->processNodes();
	    }
          }
        }
      }
    }
  }
}

static cv::VideoCapture capture;
static QSharedPointer<br::Transform> transform;
static QSharedPointer<br::Distance> distance;

void Eye::realtimeWatch(void)
{
  cv::Mat vis, frame;
  Photo* photo;
  while (capture.read(frame))
  {
    br::Template frame_query(frame);
    br::TemplateList querylist;
    querylist.push_back(frame_query);
    querylist >> *transform;
    cv::Mat vis = frame.clone();
    cv::imshow("TheEye", vis);

    int c  = cv::waitKey(100);
    if ((c == 'c') || (c == 'q') || (c == 27)) {
      return;
    }

    for (int detect_idx = 0; detect_idx < querylist.count(); detect_idx++)
    {
      std::string name;
      std::string node_id;
      br::Template query = querylist.at(detect_idx);
      //double min_distance = std::numeric_limits<double>::min();
      double min_distance = 0;
      bool match = false;
      for (auto& node_pair : eye_facebase->nodes)
      {
        Node *node = node_pair.second;
        node->faces_mutex.lock();
	std::list<Face*>::iterator it;
	for (it = node->faces.begin(); it != node->faces.end(); ++it)
        {
	  Face *face = *it;
	  if (face->detect)
	  {
            //double dis = 0;
	    Photo* photo = eye_facebase->photos[face->photo_id];
	    //printf("%f %s\n", dis, face->name.c_str());
            float dis = distance->compare(face->face_template, query);
            if (dis > min_distance)
            {
              min_distance = dis;
	      name.assign(face->name);
	      node_id.assign(face->node_id);
	      match = true;
            }
          }
        }
        node->faces_mutex.unlock();
      }
      if (match && min_distance > 2.5)
        printf ("\t\t\t\t%s p:%4.2f\n", name.c_str(), min_distance, detect_idx);
      else if (match && min_distance > 2.0)
        printf ("\t\t\t%s p:%4.2f i:%d\n", name.c_str(), min_distance, detect_idx);
      else if (match && min_distance > 1.5)
        printf ("\t\t%s p:%4.2f i:%d\n", name.c_str(), min_distance, detect_idx);
      else if (match && min_distance > 1.0)
        printf ("\t%s p:%4.2f i:%d\n", name.c_str(), min_distance, detect_idx);
      else if (match && min_distance > 0.5)
        printf ("%s p:%4.2f i:%d\n", name.c_str(), min_distance, detect_idx);

    }
  }
}

void Eye::recognize(void)
{
  cv::Mat vis, frame;
  capture.read(frame);
  br::Template frame_query(frame);
  br::TemplateList querylist;
  querylist.push_back(frame_query);
  querylist >> *transform;
  vis = frame.clone();
  double long_term_dis;
  std::string long_term_name;

  for (int detect_idx = 0; detect_idx < querylist.count(); detect_idx++)
  {
    std::string name;
    std::string node_id;
    br::Template query = querylist.at(detect_idx);
    const QRect faceroi = query.file.get<QRect>("FrontalFace");
    //double min_distance = std::numeric_limits<double>::min();
    double min_distance = 0;
    bool match = false;
    for (auto& node_pair : eye_facebase->nodes)
    {
      Node *node = node_pair.second;
      node->faces_mutex.lock();
      std::list<Face*>::iterator it;
      for (it = node->faces.begin(); it != node->faces.end(); ++it)
      {
        Face *face = *it;
        if (face->detect)
        {
          //double dis = 0;
          Photo* photo = eye_facebase->photos[face->photo_id];
          //printf("%f %s\n", dis, face->name.c_str());
          float dis = distance->compare(face->face_template, query);
          if (dis > min_distance)
          {
            min_distance = dis;
            name.assign(face->name);
            node_id.assign(face->node_id);
            match = true;
          }
        }
      }
      node->faces_mutex.unlock();
    }
    if (match && min_distance > 2.5)
      printf ("\t\t\t\t%s p:%4.2f\n", name.c_str(), min_distance, detect_idx);
    else if (match && min_distance > 2.0)
      printf ("\t\t\t%s p:%4.2f i:%d\n", name.c_str(), min_distance, detect_idx);
    else if (match && min_distance > 1.5)
      printf ("\t\t%s p:%4.2f i:%d\n", name.c_str(), min_distance, detect_idx);
    else if (match && min_distance > 1.0)
      printf ("\t%s p:%4.2f i:%d\n", name.c_str(), min_distance, detect_idx);
    else if (match && min_distance > 0.5)
      printf ("%s p:%4.2f i:%d\n", name.c_str(), min_distance, detect_idx);

    int thick = (int)(min_distance * 3);
    cv::rectangle(vis, cv::Point( faceroi.x(), faceroi.y() ), cv::Point( faceroi.x() + faceroi.width(), faceroi.y() + faceroi.height()),cv::Scalar( 0, 255, 0),thick,8 ); 

    std::ostringstream report;
    if (match && min_distance > long_term_dis)
    {
      long_term_dis = min_distance;
      long_term_name.assign(name);
    }
    if (long_term_dis > 1.2)
    {
      putText(vis, long_term_name.c_str(), cv::Point( faceroi.x() + faceroi.width(), faceroi.y() + faceroi.height()),
                     cv::FONT_HERSHEY_PLAIN, 2,
                     cv::Scalar(0, 0, 255),
                     2, 3);
      long_term_dis *= 0.999;
    }

  }
  //double x = faceroi.x() + (faceroi.width() / 2);
  //double y = faceroi.y() + (faceroi.height() / 2);
  
  cv::imshow("FacialRecognition", vis);
  int c  = cv::waitKey(50);
  if ((c == 'c') || (c == 'q') || (c == 27)) {
    return;
  }
}

void Eye::realtimeView(void)
{
  cv::Mat vis, frame;
  Photo* photo;
  while (1)
  {
    for (auto& photo_pair : eye_facebase->photos)
    {
      photo = photo_pair.second;
      if (photo != NULL && photo->valid)
      {
        for (int face_idx = 0; face_idx < photo->face_count; face_idx++)
        {
          Face* face = photo->faces[face_idx];
          if (face != NULL && face->detect)
          {
            showFace(face);
	    for (int i = 0 ; i < 10; i++)
	    {
              recognize();
	    }
            int c = cv::waitKey(100);
	    //printf("c:%d\n", c);
            if ((c == 'c') || (c == 'q') || (c == 27))
	    {
              return;
            }
	    if ((c == 114))
	    {
	      printf("searching for matches\n");
              eye_facebase->processNodes();
	    }
          }
        }
      }
    }
  }
  
}

int Eye::run(void)
{
  transform = br::Transform::fromAlgorithm("FaceRecognition");
  distance = br::Distance::fromAlgorithm("FaceRecognition");

  //cv::Mat img = cv::imread("/home/cgreen/photobase/test", CV_LOAD_IMAGE_UNCHANGED);

  //cv::namedWindow("MyWindow", CV_WINDOW_AUTOSIZE); //create a window with the name "MyWindow"
  //cv::imshow("MyWindow", img); //display the image which is stored in the 'img' in the "MyWindow" window

  //cv::waitKey(5000); //wait infinite time for a keypress

  //cv::destroyWindow("MyWindow"); //destroy the window with the name, "MyWindow"
  //return 0;


  capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
  capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

  capture.open(0);

  if (!capture.isOpened()) {
    std::cerr << "---(!)Error opening video capture\n";
    return 0;
  }
 
  while (1)
  {
    int mode = 0;
    printf("Select mode\n\
	   (1) Refresh Processing\n \
	   (2) Facial Recognition\n \
	   (3) View Photos\n \
	   (4) Database status\n");
    std::cin >> mode;
    if (mode == 0)
      break;
    else if (mode == 1)
      eye_facebase->processNodes();
    else if (mode == 2)
      realtimeWatch();
    else if (mode == 3)
      cycleAllPhotos();
    else if (mode == 4)
      status();
    else if (mode == 5)
      realtimeView();
    else if (mode == 10)
      viewNewPhotos();
  }
}
