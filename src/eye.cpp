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

int Eye::run(void)
{
  QSharedPointer<br::Transform> transform = br::Transform::fromAlgorithm("FaceRecognition");
  QSharedPointer<br::Distance> distance = br::Distance::fromAlgorithm("FaceRecognition");

  //cv::Mat img = cv::imread("/home/cgreen/photobase/test", CV_LOAD_IMAGE_UNCHANGED);

  //cv::namedWindow("MyWindow", CV_WINDOW_AUTOSIZE); //create a window with the name "MyWindow"
  //cv::imshow("MyWindow", img); //display the image which is stored in the 'img' in the "MyWindow" window

  //cv::waitKey(5000); //wait infinite time for a keypress

  //cv::destroyWindow("MyWindow"); //destroy the window with the name, "MyWindow"
  //return 0;

  cv::VideoCapture capture;

  capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
  capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);

  capture.open(0);

  if (!capture.isOpened()) {
    std::cerr << "---(!)Error opening video capture\n";
    return 0;
  }
 
  cv::Mat vis, frame;
  Photo* photo;
  
  /*while(photo = eye_facebase->getNewPhoto())
  {
    br::Template photo_template(photo->file_path.c_str());
    
    for (int face_idx = 0; face_idx < photo->face_count; face_idx++)
    {
      Face* face = photo->faces[face_idx];
      if (face->detect)
      {
        vis = cv::imread(photo->file_path.c_str(), CV_LOAD_IMAGE_UNCHANGED);

        cv::Point fe(face->detect_x, face->detect_y), se(face->tag_x, face->tag_y);
        cv::circle(vis, fe, 20.0, cv::Scalar(255, 0, 0));
        cv::circle(vis, se, 20.0, cv::Scalar(0, 0, 255));

	std::ostringstream report;
	report << "name" << face->name;

	putText(vis, report.str().c_str(), cv::Point(10, 30),
                       cv::FONT_HERSHEY_PLAIN, 2,
                       cv::Scalar::all(255),
                       2, 3);

        cv::imshow("TheEye", vis);

        int c = cv::waitKey(100000);
        if ((c == 'c') || (c == 'q') || (c == 27)) {
            break;
        }
      }
    }
  }*/
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
        break;
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
	std::list<Face*>::iterator it;
	for (it = node->faces.begin(); it != node->faces.end(); ++it)
        {
	  Face *face = *it;
	  if (face->detect)
	  {
            //double dis = 0;
	    Photo* photo = eye_facebase->photos[face->photo_id];
            //br::Template photo_template(photo->file_path.c_str());
            //cv::imshow("TheEye", photo_template.m());
            //c = cv::waitKey(100);
            //br::TemplateList face_templates;
            //face_templates.push_back(photo_template);
            //face_templates >> *transform;
	    //printf("count:%d %d\n", face_templates.count(), face->face_detect_idx);
            //br::Template detect_template = face_templates.at(face->face_detect_idx);
            //float dis = distance->compare(detect_template, query);
            float dis = distance->compare(face->face_template, query);
	    printf("%f %s\n", dis, face->name.c_str());
            if (dis > min_distance)
            {
              min_distance = dis;
	      name.assign(face->name);
	      node_id.assign(face->node_id);
	      match = true;
            }
          }
        }
      }
      if (match)
        printf ("%s p:%f i:%d\n", name.c_str(), min_distance, detect_idx);
    }
  }
}
