#pragma once
#include <drogon/HttpController.h>

namespace api {
   class LibraryController : public drogon::HttpController<LibraryController> {
      public:
          METHOD_LIST_BEGIN
          ADD_METHOD_TO(LibraryController::refresh, "/api/refresh", drogon::Post);
          METHOD_LIST_END

          void refresh(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback);
   };
}
