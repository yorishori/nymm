// object abstraction definition
// sql -> db and db -> sql
// a public api to interface with the abstraction layer
#include <filesystem>
#include <trantor/utils/Logger.h>

#include "library.h"
#include "library_internal.h"

namespace library {
   Library::~Library() = default;

   Library::Library(const std::string& dbPath, const std::string& libPath) {
      LOG_DEBUG << "   [Library][Init Object Start]";

      std::filesystem::path dbP(dbPath);
      std::filesystem::path libP(libPath);

      if (!std::filesystem::exists(dbP)) {
         const std::string errMsg = "Error creating [library::Library]. DB path doesn't exist: " + dbPath;
         
         LOG_DEBUG << "   [Library][Controlled Exception]-> " + errMsg;
         throw std::runtime_error(errMsg);
      }

      if (!std::filesystem::exists(libP) && !std::filesystem::is_directory(libP)) {
         const std::string errMsg = "Error creating [library::Library]. Lib path doesn't exist or isn't directory: " 
            + dbPath;
         
         LOG_DEBUG << "   [Library][Controlled Exception]-> " + errMsg;
         throw std::runtime_error(errMsg);
      }

      if (std::filesystem::is_directory(dbP)) {
         dbP = std::filesystem::path((dbP / "library.db").string());
      }

      this->sql_ = std::make_unique<library::Sql>(dbP);
      this->sql_->initDb();

      this->files_ = std::make_unique<library::Files>(libP, *this->sql_);
      
      LOG_DEBUG << "   [Library][Init Object End]";
   }

   void Library::refreshLibrary() {
      LOG_DEBUG << "   [Library][Function Start][refreshLibrary()]";
      this->files_->refreshLibrary();
      LOG_DEBUG << "   [Library][Function End][refreshLibrary()]";

   }
}


