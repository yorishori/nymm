#include <filesystem>
#include <stdexcept>
#include <cstdlib>

#include <drogon/HttpAppFramework.h>

#include "library.h"

static void validateDir(const std::string &path, const std::string &name) {
   if (!std::filesystem::exists(path))
      throw std::runtime_error(name + " does not exist: " + path);
   if (!std::filesystem::is_directory(path))
      throw std::runtime_error(name + " is not a directory: " + path);
}

int main() {
   drogon::app().setLogLevel(trantor::Logger::kTrace);
   drogon::app().loadConfigFile("./config.json");
   auto custom = drogon::app().getCustomConfig();

   std::string libPath = custom["lib_path"].asString(); 
   std::string dbPath = custom["db_path"].asString();

   try {
      validateDir(libPath, "lib_path");
   } catch (const std::exception &e) {
      LOG_ERROR << "Startup error: " << e.what();
      return EXIT_FAILURE;
   }
   
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
   
   drogon::app().run();
   return 0;
}


