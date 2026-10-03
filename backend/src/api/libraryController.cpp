#include <stdexcept>
#include <cstdlib>

#include <drogon/HttpController.h>
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpResponse.h>

#include "controllers.h"
#include "plugins.h"
#include "library.h"

using namespace drogon;

namespace api {
   void LibraryController::refresh(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
   {
      try {
         auto &lib = app().getPlugin<LibraryPlugin>()->get();
         lib.refreshLibrary();

         callback(HttpResponse::newHttpResponse());
      } catch (const std::exception &e) {
         LOG_ERROR << "Refresh failed: " << e.what();

         Json::Value err;
         err["error"] = e.what();
         auto resp = HttpResponse::newHttpJsonResponse(err);
         resp->setStatusCode(drogon::k500InternalServerError);
         callback(resp);
      }
   }

   void LibraryController::getData(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
   {
      
   }
}
