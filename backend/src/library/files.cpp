/* ============================
 * File Layer
 * ============================
 * This will be the main and only entry point that directly manipulates files.
 * Reading consists in mirroring the library to the DB.
 * Writing has a more robust procedure and has to comply with several protocols before actually modifying the file's tags or 
 * location.
 *
 * This will never delete a file or leave an empty tag list. Updating tags will garantee that primary tags are never empty or 
 * blank.
 * Updates will never come directly from elsewhere in the app, only from the changes DB. `sql.cpp` will ensure that data written
 * to these tables is correct and files.cpp will ensure first that these writes are correct and well formatted.
 *
 */
#include <string>
#include <vector>
#include <filesystem>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

#include <trantor/utils/Logger.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>

#include "library.h"
#include "library_internal.h"

#include <taglib/tdebuglistener.h>



namespace library {
   //=======================
   //     Read Logic
   //=======================
   bool hasAlbumArt(const TagLib::File* f) {
      return f && f->complexPropertyKeys().contains("PICTURE");
   }

   Track readTrack(const std::filesystem::path& path){
      TagLib::FileRef f(path.string().c_str());

      if (f.isNull() || !f.tag()){
         const std::string errMsg = "Error in reading file [readTrack()]: `" + path.string() + "`";
         throw library::InvalidTrackException(errMsg);
      }

      TagLib::PropertyMap props = f.file()->properties();

      auto get = [&props](const char* key) -> std::string {
         auto it = props.find(key);
         return (it != props.end() && !it->second.isEmpty()) ? it->second.front().to8Bit(true) : std::string{};
      };

      bool hasArt = hasAlbumArt(f.file());
      
      return Track{
         .trackId = 0,
         .albumId = 0,
         .title           = get("TITLE"),
         .album           = get("ALBUM"),
         .albumArtist     = get("ALBUMARTIST"),
         .trackNumber     = get("TRACKNUMBER"),
         .discNumber      = get("DISCNUMBER"),
         .date            = get("DATE"),
         .genre           = get("GENRE"),
         .composer        = get("COMPOSER"),
         .compilation     = get("COMPILATION") == "1",
         .hasArt          = hasArt,
         .path            = path.string(),
         .isrc            = get("ISRC"),
         .asin            = get("ASIN"),
         .bpm             = get("BPM"),
         .copyright       = get("COPYRIGHT"),
         .encodeBy        = get("ENCODEDBY"),
         .mood            = get("MOOD"),
         .media           = get("MEDIA"),
         .label           = get("LABEL"),
         .catalogNumber   = get("CATALOGNUMBER"),
         .barCode         = get("BARCODE"),
         .titleSort       = get("TITLESORT"),
         .albumSort       = get("ALBUMSORT"),
         .artistSort      = get("ARTISTSORT"),
         .albumArtistSort = get("ALBUMARTISTSORT"),
         .composerSort    = get("COMPOSERSORT"),
         .mbTrackId       = get("MUSICBRAINZ_TRACKID"),
         .mbAlbumId       = get("MUSICBRAINZ_ALBUMID"),
         .navidromeId     = get("NAVIDROME_ID"),
      };
   }

   void extractorFun(Q<std::filesystem::path>& pathQ, Q<Track>& trackQ) {
      while (true) {
         auto paths = pathQ.popBatch(32, 1000ms);
         if (paths.empty() && pathQ.isQDone()) break;
         for (auto& p : paths) {
            try {
               trackQ.push(readTrack(p));
            } catch (const std::exception& e) {
               LOG_TRACE << "   [Files][extractorFun]-> Exception in [readTrack()], skipping: " << e.what();
            }
         }
      }
   }

   void Files::refreshLibrary() {
      LOG_DEBUG << "   [FILES][Function Start][refreshLibrary()]";

      Q<std::filesystem::path> pathQ(50000);
      Q<Track> trackQ(4000);

      std::thread fsReader([&] {
         for (const auto& inode : 
            std::filesystem::recursive_directory_iterator(
               this->musicLibPath_.string(),
               std::filesystem::directory_options::skip_permission_denied
            )
         ) {
            if (inode.is_regular_file()) pathQ.push(inode.path());
         }
         pathQ.finish();
      });

      std::thread dbWriter([&] {
         while (true) {
            auto tracks = trackQ.popBatch(500, 500ms);
            if (!tracks.empty()) this->sql_.insertTracks(tracks);
            if (tracks.empty() && trackQ.isQDone()) break;
         }
      });

      unsigned n = std::max(1u, std::thread::hardware_concurrency() - 2);
      std::vector<std::thread> extractors;
      for (unsigned i = 0; i < n; ++i)
         extractors.emplace_back(extractorFun, std::ref(pathQ), std::ref(trackQ));
      
      fsReader.join(); 
      for (auto& e : extractors) e.join();
      trackQ.finish();
      dbWriter.join();

      LOG_DEBUG << "   [FILES][Function End][refreshLibrary()]";
   }
}


//=======================
//     Write Logic
//=======================

// filer scanner -> taglib -> writes to db.
