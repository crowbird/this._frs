#ifndef FACEBASE_H
#define FACEBASE_H

#include <iostream>
#include <map>
#include <queue>
#include <mutex>

#include <photo.h>
#include <face.h>
#include <node.h>

#define DISTANCE_THRESHOLD 50

class FaceBase
{
  public:
    FaceBase(void);
    void detectFaces(std::string /*photo_id*/);
    int newPhoto(std::string /*photo_id*/, std::string /*link*/, int tag_count);
    Face* newFace(int /*face_idx*/, std::string /*photo_id*/, std::string /*node_id*/, double x, double y, std::string face_name);
    Photo* nextValidPhoto(void);
    Photo* nextProcessingPhoto(void);
    std::map<std::string, Node*> nodes;
    std::map<std::string, Photo*> photos;
    int face_count;
    int node_count;
    int photo_count;
    int start(void);
    int stop(void);
    void flagPhotoForProcessing(std::string /*photo_id*/);
    void flagPhotoValid(std::string /*photo_id*/);
    void processNodes(void);
  private:
    void processNode(std::string /*node_id*/);
    pthread_t facebase_thread;
    std::queue<Photo*> valid_photos;
    pthread_mutex_t valid_photos_mutex;
    pthread_cond_t valid_photos_cond;
    std::queue<Photo*> processing_photos;
    pthread_mutex_t processing_photos_mutex;
    pthread_cond_t processing_photos_cond;
    //void* run(void*);
    //Node* findNode(std::string /*node_id*/);
    Node* newNode(std::string /*node_id*/);
    //std::map<std::string, Face*> faces;
};

#endif // FACEBASE_H
