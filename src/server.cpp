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

FaceBase *server_facebase;

Server::Server(int port, FaceBase* fb)
{
  this->port = port;
  server_facebase = fb;
}

int Server::run()
{
  crow::SimpleApp app;

  CROW_ROUTE(app, "/new_photos")
  .methods("POST"_method)
  ([](const crow::request& req){

    auto photosMetadata = crow::json::load(req.body);
    
    if (!photosMetadata)
    {
      // unable 
      return crow::response(400);
    }
    try
    {
      printf("Got JSON\n");
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
        server_facebase->newPhoto(photo_id, photo_link, photosMetadata[photo]["tags"]["data"].size());
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
            server_facebase->newFace(tag, photo_id, node_id, (x/100)*width, (y/100)*height, face_name);
          }
        }
        server_facebase->detectFaces(photo_id);

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

    }
    catch (std::exception e)
    {
      return crow::response(400);
    }

    return crow::response(200);
  });

  

  app.port(port)
  //.multithreaded()
  .run();

  return 0;
}

