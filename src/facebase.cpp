#include <QString>
#include <iostream>
#include <limits>
#include <queue>
#include <math.h>
#include <openbr/openbr_plugin.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <facebase.h>

FaceBase::FaceBase()
{
}

void FaceBase::detectFaces(std::string photo_id)
{
  QSharedPointer<br::Transform> face_transform = br::Transform::fromAlgorithm("Open+Cascade(FrontalFace)+ASEFEyes");
  br::TemplateList querylist;
  br::Template photo_template(photos[photo_id]->file_path.c_str());

  querylist.push_back(photo_template);
  querylist >> *face_transform;
  printf("%s detected %u faces\n", photo_id.c_str(), querylist.count());
  for (int i = 0; i < querylist.count(); i++)
  {
    br::Template face_template = querylist.at(i);
    
    const QRect faceroi = face_template.file.get<QRect>("FrontalFace");
    double x = faceroi.x() + (faceroi.width() / 2);
    double y = faceroi.y() + (faceroi.height() / 2);
    printf("face at %f,%f\n", x,y);

    // find close tag
    double min_distance = std::numeric_limits<double>::max();
    int min_face_idx = 0;
    for(int face_idx = 0; face_idx < photos[photo_id]->face_count; face_idx++)
    {
      Face* face = photos[photo_id]->faces[face_idx];;
      double distance = sqrt(pow(abs(face->x - x),2) + pow(abs(face->y - y),2));
      if (distance < min_distance)
      {
        min_distance = distance;
        min_face_idx = face_idx;
      }
    }
    Face* face = photos[photo_id]->faces[min_face_idx];
    printf("tag at %f,%f dis - %f for %s\n", face->x, face->y, min_distance, face->name.c_str());
  }
  //photos[photo_id]->valid = true;
  //valid_photos.push(photos[photo_id]);
}

void FaceBase::newPhoto(std::string photo_id, std::string link, int face_count)
{
  if (photos[photo_id] == NULL)
  {
    photos[photo_id] = new Photo(photo_id, link, face_count);
  }

  // download photo
  photos[photo_id]->downloadLink();
}

Node* FaceBase::newNode(std::string node_id)
{
  Node* node = nodes[node_id];
  if (node == NULL)
  {
    node = new Node(node_id);
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

  Node* node = newNode(node_id);
  Face* face = photo->faces[face_idx];
  if (face != NULL)
  {
    printf("face already exist\n");
    // face already in database
    return NULL;
  }

  face = new Face(face_idx, node_id, x, y, face_name);
  node->addFace(face);

  nodes[node_id] = node;
  photos[photo_id]->faces[face_idx] = face;
  return face;
}
