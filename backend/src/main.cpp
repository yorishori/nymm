#include <filesystem>
#include <cstdlib>

#include <drogon/HttpAppFramework.h>

#include "library.h"

int main() {
   std::string libPath = "/mnt/nfs/music/library_old/";
   std::string dbPath = "/home/yori/nymm/bin/";    
   library::Library lib(dbPath, libPath);
   
   drogon::app().registerHandler(
      "/api/refresh",
      [&lib](const drogon::HttpRequestPtr &, std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
         try {
            lib.refreshLibrary();
         } catch (const std::exception &e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            throw;
         }
         callback(drogon::HttpResponse::newHttpResponse());
      },
      {drogon::Post});

   drogon::app().loadConfigFile("./config.json");
   
   drogon::app().setLogLevel(trantor::Logger::kTrace);
   drogon::app().run();
   
   return 0;
}
