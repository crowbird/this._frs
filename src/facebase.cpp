#include <QString>
#include <iostream>
#include <limits>
#include <queue>
#include <math.h>
#include <assert.h>

// OpenBR/OpenCV
#include <openbr/openbr_plugin.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

// frs
#include <facebase.h>

FaceBase::FaceBase()
{
  pthread_mutex_init(&processing_photos_mutex, NULL);
  pthread_mutex_init(&valid_photos_mutex, NULL);
  pthread_cond_init(&processing_photos_cond, NULL);
  pthread_cond_init(&valid_photos_cond, NULL);
}

static void* facebase_run(void *context)
{
  FaceBase* fb = (FaceBase*)context;
  while(1)
  {
    //printf("process photo\n");
    // wait for new photo
    Photo* photo = fb->nextProcessingPhoto();

    // detect faces
    fb->detectFaces(photo->id);
    fb->flagPhotoValid(photo->id);
  }
  return NULL;
}

int FaceBase::start()
{
  int ret = pthread_create(&facebase_thread, NULL, facebase_run, this);
  assert(ret == 0);
  printf("facebase started...\n");
  return ret;
}

int FaceBase::stop()
{
  pthread_join(facebase_thread, NULL);
  printf("facebase stopped...\n");
  return 0;
}

void FaceBase::flagPhotoForProcessing(std::string photo_id)
{
  pthread_mutex_lock(&processing_photos_mutex);
  processing_photos.push(photos[photo_id]);
  photo_count++;
  pthread_cond_signal(&processing_photos_cond);
  pthread_mutex_unlock(&processing_photos_mutex);
}

void FaceBase::flagPhotoValid(std::string photo_id)
{
  photos[photo_id]->valid = true;
  pthread_mutex_lock(&valid_photos_mutex);
  valid_photos.push(photos[photo_id]);
  pthread_cond_signal(&valid_photos_cond);
  pthread_mutex_unlock(&valid_photos_mutex);
}

Photo* FaceBase::nextProcessingPhoto(void)
{
  pthread_mutex_lock(&processing_photos_mutex);
  while (processing_photos.empty())
  {
    pthread_cond_wait(&processing_photos_cond, &processing_photos_mutex);
  }

  Photo *photo = processing_photos.front();
  processing_photos.pop();
  pthread_mutex_unlock(&processing_photos_mutex);
  return photo;
}

Photo* FaceBase::nextValidPhoto(void)
{
  pthread_mutex_lock(&valid_photos_mutex);
  while (valid_photos.empty())
  {
    pthread_cond_wait(&valid_photos_cond, &valid_photos_mutex);
  }

  Photo *photo = valid_photos.front();
  valid_photos.pop();
  pthread_mutex_unlock(&valid_photos_mutex);
  return photo;
}

void FaceBase::detectFaces(std::string photo_id)
{
  Photo* photo = photos[photo_id];
  //printf("detectFace %p d:%d c:%d\n", photo, (photo == NULL ? 0 : photo->downloaded), (photo == NULL ? 0 : photo->face_count));
  if (photo == NULL || !photo->downloaded || photo->processed)
  {
    return;
  }

  try
  {
    //QSharedPointer<br::Transform> face_transform = br::Transform::fromAlgorithm("Open+Cascade(FrontalFace)+ASEFEyes");
    QSharedPointer<br::Transform> face_transform = br::Transform::fromAlgorithm("FaceRecognition");
    br::Template photo_template(photos[photo_id]->file_path.c_str());

    //br::TemplateList face_templates;
    //face_templates.push_back(photo_template);
    //face_templates >> *face_transform;
    
    photo->face_templates.push_back(photo_template);
    photo->face_templates >> *face_transform;
    
    //printf("%s detected %u faces\n", photo_id.c_str(), photo->face_templates.count());
    for (int i = 0; i < photo->face_templates.count(); i++)
    {
      //br::Template detect_template = face_templates.at(i);
      br::Template detect_template = photo->face_templates.at(i);
      
      const QRect faceroi = detect_template.file.get<QRect>("FrontalFace");
      double x = faceroi.x() + (faceroi.width() / 2);
      double y = faceroi.y() + (faceroi.height() / 2);
      //printf("face at %f,%f\n", x,y);

      // find close tag
      double min_distance = std::numeric_limits<double>::max();
      int min_face_idx = 0;
      for(int face_idx = 0; face_idx < photos[photo_id]->face_count; face_idx++)
      {
        Face* face = photos[photo_id]->faces[face_idx];
        if (face != NULL)
        {
          assert(face != NULL);
          
          double x_dis_p = pow(abs(face->tag_x - x),2);
          double y_dis_p = pow(abs(face->tag_y - y),2);
          double distance = sqrt(x_dis_p + y_dis_p);
          //double distance = sqrt(pow(abs(face->tag_x - x),2) + pow(abs(face->tag_y - y),2));
          if (distance < min_distance)
          {
            min_distance = distance;
            min_face_idx = face_idx;
          }
        }
      }
      Face* face = photos[photo_id]->faces[min_face_idx];
      if(face != NULL)
      {
        if (min_distance < DISTANCE_THRESHOLD)
        {
          face->markDetect(x,y, min_distance, i);
          face_count++;
          face->face_template = detect_template;
          Node* node = newNode(face->node_id);
          assert(node != NULL);
          /*std::list<Face*>::iterator it;
          for (it = node->faces.begin(); it != node->faces.end(); ++it)
          {
            Face *compare_face = *t;

          }*/
          node->addFace(face);
          nodes[face->node_id] = node;
        }
        //printf("tag at %f,%f dis - %f for %s\n", face->tag_x, face->tag_y, min_distance, face->name.c_str());
      }
    }
  }
  catch (std::exception e)
  {
    //std::cout << "detectFaces Exception:" << e;
  }
  photo->processed = true;
}

void FaceBase::processNodes(void)
{
  for (auto& node_pair : nodes)
  {
    std::string node_id = node_pair.first;
    processNode(node_id);
  }
}

void FaceBase::processNode(std::string node_id)
{
  try
  {
    QSharedPointer<br::Distance> distance = br::Distance::fromAlgorithm("FaceRecognition");
    Node* node = nodes[node_id];
    assert(node != NULL);
    node->faces_mutex.lock();
    std::list<Face*>::iterator it0;
    std::list<Face*>::iterator it1;
    for (it0 = node->faces.begin(); it0 != node->faces.end() && std::next(it0) != node->faces.end(); ++it0)
    {
      Face *face0 = *it0;
      face0->match_count = 0;
      if (face0->detect)
      {
        for (it1 = std::next(it0); it1 != node->faces.end(); ++it1)
        {
          Face *face1 = *it1;
          if (face1->detect)
          {
            double d = distance->compare(face0->face_template, face1->face_template);
            if (d > 1.5)
            {
              face0->match_count++;
              face1->match_count++;
              printf("%s match %s to %s\n", face0->name.c_str(), face0->photo_id.c_str(), face1->photo_id.c_str());
            }
          }
        }
      }
    }
    node->faces_mutex.unlock();
  }
  catch (std::exception e)
  {
    std::cout << "processNode exception";
  }
}

int FaceBase::newPhoto(std::string photo_id, std::string link, int face_count)
{
  if (photos[photo_id] == NULL)
  {
    photos[photo_id] = new Photo(photo_id, link, face_count);
  }
  else
  {
    return -1;
  }

  // download photo
  photos[photo_id]->downloadLink();
  return 0;
}

Node* FaceBase::newNode(std::string node_id)
{
  Node* node = nodes[node_id];
  if (node == NULL)
  {
    node = new Node(node_id);
    node_count++;
  }
  nodes[node_id] = node;
  return node;
}

Face* FaceBase::newFace(int face_idx, std::string photo_id, std::string node_id, double x, double y, std::string face_name)
{
  Photo* photo = photos[photo_id];
  if (photo == NULL)
  {
    printf("Can't add Face dont know photo\n");
    // photo must already be in FaceBase to add face
    return NULL;
  }

  Face* face = photo->faces[face_idx];
  if (face != NULL)
  {
    //printf("face already exist\n");
    // face already in database
    return NULL;
  }

  face = new Face(face_idx, node_id, photo_id, x, y, face_name);

  if (face != NULL)
  {
    photos[photo_id]->faces[face_idx] = face;
    photos[photo_id]->face_count++;
  }
  else
  {
    printf("Error: failed construct Face object\n");
  }
  return face;
}
