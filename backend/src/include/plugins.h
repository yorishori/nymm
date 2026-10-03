#pragma once
#include <drogon/HttpAppFramework.h>

#include "library.h"


class LibraryPlugin : public drogon::Plugin<LibraryPlugin> {
   public:
      void initAndStart(const Json::Value &config) override {
         lib_ = std::make_unique<library::Library>(
            config["db_path"].asString(),
            config["library_path"].asString()
         );
      }
      void shutdown() override {}
      library::Library &get() { return *lib_; }
   private:
      std::unique_ptr<library::Library> lib_;
};
