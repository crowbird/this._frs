#include <QString>
#include <iostream>
#include <openbr/openbr_plugin.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <facebase.h>

FaceBase::FaceBase()
{
}

void FaceBase::newPhoto(std::string photo_id, std::string link)
{
  if (photos[photo_id] == NULL)
  {
    printf("creating photo\n");
    photos[photo_id] = new Photo(photo_id, link);
  }
  else
  {
    printf("photo already exist\n");
  }

  // download photo
  photos[photo_id]->downloadLink();

  

  // detect faces
  br::TemplateList querylist;
  // match faces to tags
  // append to node reference to face
  // while adding new nodes when one does
  // not exist.
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

Face* FaceBase::newFace(std::string face_id, std::string photo_id, std::string node_id, float x, float y)
{
  /*Photo* photo = photos[photo_id];
  Node* node = newNode(node_id);
  Face* face = faces[face_id];
  if (face != NULL)
  {
    // face already in database
    return NULL;
  }

  if (photo == NULL)
  {
    // photo must already be in FaceBase to add face
    return NULL;
  }

  face = new Face(face_id, photo, node, x, y);
  node.addFace(face);

  nodes[node_id] = node;
  faces[face_id] = face;*/
  return NULL;
}
