#include <typeinfo>
#include <QString>
#include <iostream>
#include <openbr/openbr_plugin.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <crow_all.h>

#include <server.h>
#include <facebase.h>
#include <eye.h>


Server::Server(int port, FaceBase* fb)
{
  this->port = port;
  this->facebase = fb;
}

// ugg C11/crow bug
static Server *server;

static void *server_run(void *context)
{
  server = (Server*)context;
  crow::SimpleApp app;

  CROW_ROUTE(app, "/new_photos")
  .methods("POST"_method)
  ([](const crow::request& req){

    bool success = false;
    auto photosMetadata = crow::json::load(req.body);
    
    server->facebase_mutex.lock();
    if (!photosMetadata)
    {
      // unable 
      success = false;
    }
    try
    {
      printf("MORE DATA!\n");
      //std::cout << photosMetadata << "\n";
      for(int photo = 0; photo < photosMetadata.size(); photo++)
      {
        const crow::json::rvalue *photoMetadata = &photosMetadata[photo];

        for (auto& x: *photoMetadata)
        {
          //std::cout << "\nkey:" << x.key() << " value: " << x << "\n";
        }

        std::string photo_id((*photoMetadata)["id"].s());
        std::string photo_link((*photoMetadata)["images"][0]["source"].s());
        int height = (*photoMetadata)["images"][0]["height"].i();
        int width = (*photoMetadata)["images"][0]["width"].i();
	int face_count = 0;
        for (int tag = 0; tag < photosMetadata[photo]["tags"]["data"].size(); tag++)
	{
          if (photosMetadata[photo]["tags"]["data"][tag].has("id"))
	  {
	    face_count++;
	  }
	}
        int ret = server->facebase->newPhoto(photo_id, photo_link, face_count);
	if (ret < 0)
	  break;
        for (int tag = 0; tag < photosMetadata[photo]["tags"]["data"].size(); tag++)
        {
          //std::cout << "\ntag" << photosMetadata[photo]["tags"]["data"][tag] << "\n";
          double x = 0; double y = 0;
          if (photosMetadata[photo]["tags"]["data"][tag].has("x"))
          {
            x = photosMetadata[photo]["tags"]["data"][tag]["x"].d();
          }
          if (photosMetadata[photo]["tags"]["data"][tag].has("y"))
          {
            y = photosMetadata[photo]["tags"]["data"][tag]["y"].d();
          }
          if (photosMetadata[photo]["tags"]["data"][tag].has("id"))
          {
            std::string node_id(photosMetadata[photo]["tags"]["data"][tag]["id"].s());
            std::string face_name(photosMetadata[photo]["tags"]["data"][tag]["name"].s());
	    //printf("add face\n");
            server->facebase->newFace(tag, photo_id, node_id, (x/100)*width, (y/100)*height, face_name);
          }
        }
	//printf("detect faces\n");
	server->facebase->flagPhotoForProcessing(photo_id);

        /*if(photosMetadata[item].t() == crow::json::type::List)
        {
          std::cout << "\nList\n";
          for(int i = 0; i < photosMetadata[item].size(); i++)
          {
            std::cout << "\n" << photosMetadata[item][i];
          }
        }*/

        /*for(auto& e : photosMetadata[item]) {
          std::cout << e.key();
        }*/
        
        //std::cout << "\n" << photosMetadata[item] << "\n";
        //std::cout << "\nlink - " << photosMetadata[item]["link"] << "\n";
        //crow::json::rvalue link = photosMetadata[item]["link"];
        //std::cout << "\nfrom\n"; // - " << photosMetadata[item]["from"] << "\n";
        //std::cout << "  name - " << photosMetadata[item]["from"]["name"] << "\n";
        //std::cout << "  id - " << photosMetadata[item]["from"]["id"] << "\n";
        //std::cout << "\ntags\n"; // - " << photosMetadata[item]["tags"] << "\n";
        //std::cout << "  data - " << photosMetadata[item]["tags"]["data"] << "\n";
        ////std::cout << "    name - " << photosMetadata[item]["tags"]["data"]["name"] << "\n";
        //std::cout << "  paging - " << photosMetadata[item]["tags"]["paging"] << "\n";
        //std::cout << "\nid - " << photosMetadata[item]["id"] << "\n";
        //break;
      }
      success = true;
    }
    catch (std::exception e)
    {
      success = false;
    }

    server->facebase_mutex.unlock();

    if (!success)
      return crow::response(400);
    return crow::response(200);
  });

  

  app.port(server->port)
  .multithreaded()
  .run();

  return NULL;
}

int Server::start()
{
  return pthread_create(&server_thread, NULL, server_run, this);
}

int Server::stop(void)
{
  pthread_join(server_thread, NULL);
  return 0;
}

