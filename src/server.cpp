#include <typeinfo>

#include <crow_all.h>

#include <server.h>
#include <facebase.h>

Server::Server(int port, void (*RXPhotoCallback)(Photo*))
{
  this->port = port;
  this->RXPhotoCallback = RXPhotoCallback;
}

FaceBase facebase;

int Server::start()
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
        std::string photo_id;
        std::string photo_link;
        const crow::json::rvalue *photoMetadata = &photosMetadata[photo];

        // will always be true
        // if(photoMetadata.t() == crow::json::type::Object)
        
        for (auto& x: *photoMetadata)
        {
          std::cout << "\nkey:" << x.key() << " value: " << x << "\n";
        }

        photo_id.assign((*photoMetadata)["id"].s());
        //std::cout << "\nid" << photo_id << "\n";
        photo_link.assign((*photoMetadata)["images"][0]["source"].s());
        //std::cout << "\nlink" << photo_link << "\n";
        facebase.newPhoto(photo_id, photo_link);
        /*for (int tag = 0; tag < photosMetadata[item]["tags"]["data"].size(); tag++)
        {
        }*/

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
        break;
      }
      //printf("size:%u\n", photosMetadata.size());
      //std::cout << typeid(photosMetadata).name() << '\n';
      /*for (const auto& photoMetadata : photosMetadata)
      {
        printf("j\n");
        //std::cout << "id" << photoMetadata["id"].i() << "\n";
        //Photo(
      }*/
    }
    catch (std::exception e)
    {
      return crow::response(400);
    }

    return crow::response(200);
  });

  app.port(port).multithreaded().run();

  return 0;
}

