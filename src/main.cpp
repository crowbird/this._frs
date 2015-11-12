#include <iostream>
#include <QString>
#include <iostream>
#include <openbr/openbr_plugin.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <server.h>

static void printTemplate(const br::Template &t)
{
    const QPoint firstEye = t.file.get<QPoint>("Affine_0");
    const QPoint secondEye = t.file.get<QPoint>("Affine_1");
    printf("%s eyes: (%d, %d) (%d, %d)\n", qPrintable(t.file.fileName()), firstEye.x(), firstEye.y(), secondEye.x(), secondEye.y());
}

void newPhoto(Photo* photo)
{
}

int main(int argc, char *argv[])
{
  Server server = Server(55555, newPhoto);
  server.start(); 


    br::Context::initialize(argc, argv);
    
    br_set_property("enrollALL","true");

    // Retrieve classes for enrolling and comparing templates using the FaceRecognition algorithm
    //QSharedPointer<br::Transform> transform = br::Transform::fromAlgorithm("FaceRecognition");
    //QSharedPointer<br::Distance> distance = br::Distance::fromAlgorithm("FaceRecognition");
    QSharedPointer<br::Transform> transform = br::Transform::fromAlgorithm("Open+Cascade(FrontalFace)+ASEFEyes");

    br::Globals->enrollAll = true;

    // Initialize templates
    //br::Template queryA("../data/MEDS/img/S354-01-t10_01.jpg");
    //br::Template queryB("../data/MEDS/img/S382-08-t10_01.jpg");
    //br::Template target(argv[1]);
    br::Template target_carson("/home/cgreen/img/faces/carson/shot0001.png");
    br::Template target_ian("/home/cgreen/img/faces/ian/shot0005.png");
    br::Template target_sarah("/home/cgreen/img/faces/sarah/shot0001.jpg");

    cv::VideoCapture capture;
    capture.open(0);
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    if (!capture.isOpened()) {
            std::cerr << "---(!)Error opening video capture\n";
            exit(1);
    }

    // Enroll templates
    //queryA >> *transform;
    //queryB >> *transform;

    //printTemplate(queryA);
    //printTemplate(queryB);
    //printTemplate(target);

    cv::Mat frame, final;

    while(capture.read(frame)) {

      std::string ss = "";
      br::Template frame_query(frame);
      br::TemplateList querylist;
      querylist.push_back(frame_query);
      querylist >> *transform;
      //target_carson >> *transform;
      //target_ian >> *transform;
      //target_sarah >> *transform;

      //cv::Mat detectionResult = querylist.front().m().clone();
      //cv::imshow("Test", querylist.front().m());
      //cv::waitKey();
      cv::Mat vis = frame_query.m().clone();
      //br::Template query = querylist.front();
      //for (auto query : querylist)
      printf("%u\n", querylist.count());
      for (unsigned i = 0; i < querylist.count(); i++)
      {
        br::Template query = querylist.at(i);
        const QPoint firstEye = query.file.get<QPoint>("First_Eye");
        const QPoint secondEye = query.file.get<QPoint>("Second_Eye");
        const QRect faceroi = query.file.get<QRect>("FrontalFace");
      
        printf("%s face: (%d, %d, %d, %d)\teyes: (%d, %d) (%d, %d)\n",
              qPrintable(query.file.fileName()),
              faceroi.x(), faceroi.y(), faceroi.width(), faceroi.height(),
              firstEye.x(), firstEye.y(), secondEye.x(), secondEye.y());

        putText(vis, "a", cv::Point(10, 30),
                        cv::FONT_HERSHEY_PLAIN, 2,
                        cv::Scalar::all(255),
                        2, 3);

        cv::Point fe(firstEye.x(), firstEye.y()), se(secondEye.x(), secondEye.y());
        cv::circle(vis, fe, 5.0, cv::Scalar(255, 0, 0));
        cv::circle(vis, se, 5.0, cv::Scalar(0, 0, 255));
      }

      cv::imshow("Detector", vis);

      //cv::waitKey();

      // Compare templates
      //float comparisonA = distance->compare(target, queryA);
      //float comparisonB = distance->compare(target, queryB);
      //float carson_comparison = distance->compare(target_carson, query);
      //float ian_comparison = distance->compare(target_ian, query);
      //float sarah_comparison = distance->compare(target_sarah, query);

      // Scores range from 0 to 1 and represent match probability
      //printf("Genuine match score: %.3f\n", comparisonA);
      //printf("Impostor match score: %.3f\n", comparisonB);
      //printf("match score: %.3f\n", comparison);
      /*float largest = 0;
      if (carson_comparison > largest)
      {
        ss = "Carson";
        largest = carson_comparison;
      }
      if (ian_comparison > largest)
      {
        ss = "Ian";
        largest = ian_comparison;
      }
      if (sarah_comparison > largest)
      {
        ss = "Sarah";
        largest = sarah_comparison;
      }
      printf("%s\n", ss.c_str());*/

      //ss << comparison;

      /*putText(final, "match: " + ss.str(), cv::Point(10, 30),
                        cv::FONT_HERSHEY_PLAIN, 2,
                        cv::Scalar::all(255),
                        2, 3);*/
      //imshow("Test", detectionResult);

      int c = cv::waitKey(5);
      if ((c == 'c') || (c == 'q') || (c == 27)) {
          break;
      }
    }

    br::Context::finalize();
    return 0;
}
//! [face_recognition]
