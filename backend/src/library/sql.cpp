/* ====================
 * SQL Layer
 * ====================
 *  This will be the main and only entry point to the SQLite Database.
 *  There is no direct bridge to the DB file. `sql.cpp` will only expose specific methods to read/write from the database.
 *
 *  The tables are: tracks 
 * */
#include <string>
#include <memory>
#include <vector>
#include <filesystem>

#include <sqlite3.h>
#include <trantor/utils/Logger.h>

#include "library.h"
#include "library_internal.h"

const char* INIT_SCHEMA = R"(
DROP TABLE IF EXISTS tracks;
CREATE TABLE tracks (
    trackId          INTEGER PRIMARY KEY AUTOINCREMENT,
    albumId          INTEGER,
    title            TEXT NOT NULL,
    album            TEXT,
    albumArtist      TEXT,
    trackNumber      TEXT,
    discNumber       TEXT,
    date             TEXT,
    genre            TEXT,
    composer         TEXT,
    compilation      INTEGER NOT NULL DEFAULT 0,
    hasArt           INTEGER NOT NULL DEFAULT 0,
    path             TEXT NOT NULL,
    isrc             TEXT,
    asin             TEXT,
    bpm              TEXT,
    copyright        TEXT,
    encodeBy         TEXT,
    mood             TEXT,
    media            TEXT,
    label            TEXT,
    catalogNumber    TEXT,
    barCode          TEXT,
    titleSort        TEXT,
    albumSort        TEXT,
    artistSort       TEXT,
    albumArtistSort  TEXT,
    composerSort     TEXT,
    mbTrackId        TEXT,
    mbAlbumId        TEXT,
    navidromeId      TEXT
);
CREATE UNIQUE INDEX IF NOT EXISTS idx_tracks_path ON tracks(path);
)";

// Converting C sqlite3 library funtions into a C++ pointers with deleters.

std::unique_ptr<sqlite3, library::SqliteCloser> openDb(const std::string& dbpath) {
   sqlite3* raw = nullptr;
   if (sqlite3_open(dbpath.c_str(), &raw) != SQLITE_OK){
      std::string error = sqlite3_errmsg(raw);
      sqlite3_close(raw);

      LOG_DEBUG << "   [Sql][Controlled Exception]-> Error at [openDb()]: " + error;
      throw std::runtime_error("Error at [openDb()]: " + error);
   }

   std::unique_ptr<sqlite3, library::SqliteCloser> db(raw);

   char* sqlErr = nullptr;
   int r = sqlite3_exec(db.get(), "PRAGMA journal_mode=WAL", nullptr, nullptr, &sqlErr); 
   
   if (r != SQLITE_OK) {
      std::string err = sqlErr ? sqlErr : "Unknown error L(・o・)」: CODE " + std::to_string(r);
      sqlite3_free(sqlErr);
      
      if (r == SQLITE_NOTADB || r == SQLITE_CORRUPT) {
         LOG_DEBUG << "   [Sql][Controlled Exception]-> Invalid database file: " + err;
         throw library::InvalidDatabaseException("Invalid database file: " + err);
      }
      
      LOG_DEBUG << "   [Sql][Controlled Exception]-> Error at openDb(): " + err;
      throw std::runtime_error("Error at openDb(): " + err);
   }
   
   sqlite3_free(sqlErr);

   sqlite3_exec(db.get(), "PRAGMA synchronous=NORMAL", nullptr, nullptr, nullptr); 
   sqlite3_busy_timeout(db.get(), 3000);
   return db;
}

struct StatementFinalizer {void operator()(sqlite3_stmt* s) const noexcept {sqlite3_finalize(s);};};
std::unique_ptr<sqlite3_stmt, StatementFinalizer> prepareStatement(sqlite3* db, const std::string& sql) {
   sqlite3_stmt* raw = nullptr;
   if (sqlite3_prepare_v2(db, sql.c_str(), -1, &raw, nullptr) != SQLITE_OK) {
      std::string err = sqlite3_errmsg(db);

      LOG_DEBUG << "   [Sql][Controlled Exception]-> Error in [prepareStatement()]: " + err;
      throw std::runtime_error("Error in [prepareStatement()]: " + err); 
   }
   return std::unique_ptr<sqlite3_stmt, StatementFinalizer>(raw);
}

namespace library { 
   Sql::Sql(const std::filesystem::path& path) {
      LOG_DEBUG << "   [Sql][Init Object Start]";

      bool newDb = !std::filesystem::exists(path);

      if (!newDb && !std::filesystem::is_regular_file(path)){
         const std::string errMsg = "Error creating [library::Sql]. Path is not a file: " + path.string();

         LOG_DEBUG << "   [Sql][Controlled Exception]-> " + errMsg;
         throw std::runtime_error(errMsg);
      }

      try {
         this->db_ = openDb(path.string());
      } catch (const InvalidDatabaseException& e) {
         LOG_DEBUG << "   [Sql][Controlled Exception]-> Invalid DB File. Attempting to delete old one and create a fresh DB.";
         std::filesystem::remove(path);
         this->db_ = openDb(path.string());
         newDb = true;
         LOG_DEBUG << "   [Sql][Controlled Exception]-> Succesfully created fresh DB.";
      }

      if (newDb) {
         initDb();
      }

      LOG_DEBUG << "   [Sql][Init Object End]";
   }

   void Sql::initDb() {
      LOG_DEBUG << "   [Sql][Function Start][initDb()]";

      char* sqlErr = nullptr;
      int r = sqlite3_exec(this->db_.get(), INIT_SCHEMA, nullptr, nullptr, &sqlErr);
      if (r != SQLITE_OK){
         std::string err = sqlErr ? sqlErr : "Unknown error L(・o・)」: CODE " + std::to_string(r);
         sqlite3_free(sqlErr);

         LOG_DEBUG << "   [Sql][Controlled Exception]-> Error in [library::Sql::initDb()]:  " + err;
         throw std::runtime_error("Error in [library::Sql::initDb()]: " + err);
      }

      LOG_DEBUG << "   [Sql][Function End][initDb()]";

   }

   void Sql::insertTracks(const std::vector<Track>& trks) {
      const char* sqlStm = R"(
   INSERT INTO tracks (
       title, album, albumArtist, trackNumber, discNumber, date, genre, composer,
       compilation, hasArt, path, isrc, asin, bpm, copyright, encodeBy, mood, media, label,
       catalogNumber, barCode, titleSort, albumSort, artistSort, albumArtistSort, composerSort,
       mbTrackId, mbAlbumId, navidromeId
   ) VALUES (
       ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?
   )
   ON CONFLICT(path) DO UPDATE SET
       title=excluded.title, album=excluded.album, albumArtist=excluded.albumArtist,
       trackNumber=excluded.trackNumber, discNumber=excluded.discNumber, date=excluded.date,
       genre=excluded.genre, composer=excluded.composer, compilation=excluded.compilation,
       hasArt=excluded.hasArt, isrc=excluded.isrc, asin=excluded.asin, bpm=excluded.bpm,
       copyright=excluded.copyright, encodeBy=excluded.encodeBy, mood=excluded.mood,
       media=excluded.media, label=excluded.label, catalogNumber=excluded.catalogNumber,
       barCode=excluded.barCode, titleSort=excluded.titleSort, albumSort=excluded.albumSort,
       artistSort=excluded.artistSort, albumArtistSort=excluded.albumArtistSort,
       composerSort=excluded.composerSort, mbTrackId=excluded.mbTrackId,
       mbAlbumId=excluded.mbAlbumId, navidromeId=excluded.navidromeId;
   )";

      sqlite3_exec(this->db_.get(), "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

      try {
         auto stm = prepareStatement(this->db_.get(), sqlStm);

         for (const Track& trk : trks) {
            int i = 1;
            sqlite3_bind_text (stm.get(), i++, trk.title.c_str(),           -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.album.c_str(),           -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.albumArtist.c_str(),     -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.trackNumber.c_str(),     -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.discNumber.c_str(),      -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.date.c_str(),            -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.genre.c_str(),           -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.composer.c_str(),        -1, SQLITE_STATIC);
            sqlite3_bind_int  (stm.get(), i++, trk.compilation);
            sqlite3_bind_int  (stm.get(), i++, trk.hasArt);
            sqlite3_bind_text (stm.get(), i++, trk.path.c_str(),            -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.isrc.c_str(),            -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.asin.c_str(),            -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.bpm.c_str(),             -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.copyright.c_str(),       -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.encodeBy.c_str(),        -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.mood.c_str(),            -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.media.c_str(),           -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.label.c_str(),           -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.catalogNumber.c_str(),   -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.barCode.c_str(),         -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.titleSort.c_str(),       -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.albumSort.c_str(),       -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.artistSort.c_str(),      -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.albumArtistSort.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.composerSort.c_str(),    -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.mbTrackId.c_str(),       -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.mbAlbumId.c_str(),       -1, SQLITE_STATIC);
            sqlite3_bind_text (stm.get(), i++, trk.navidromeId.c_str(),     -1, SQLITE_STATIC);

            if (sqlite3_step(stm.get()) != SQLITE_DONE) {
               std::string err = sqlite3_errmsg(this->db_.get());

               LOG_DEBUG << "   [Sql][Controlled Exception]-> Error in [library::Sql::insertTracks()]: " + err;
               throw std::runtime_error("Error in [library::Sql::insertTracks()]: " + err);
            }

            sqlite3_reset(stm.get());
            sqlite3_clear_bindings(stm.get());
         }

         sqlite3_exec(this->db_.get(), "COMMIT;", nullptr, nullptr, nullptr);
      } catch (...) {
         sqlite3_exec(this->db_.get(), "ROLLBACK;", nullptr, nullptr, nullptr);
         throw;
      }
   }
}
