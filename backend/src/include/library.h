#pragma once
#include <string>
#include <iostream>
#include <memory>

namespace library {
   class Sql;
   class Files;

   class Track {
      public:
         // Internal Members
         const int trackId;
         const int albumId;
         // Mandatory Members
         std::string title;
         std::string album;
         std::string albumArtist;
         std::string trackNumber;
         std::string discNumber;
         std::string date;
         std::string genre;
         std::string composer;
         bool compilation = false;
         bool hasArt = false;
         std::string path;
         // Extra
         std::string isrc;
         std::string asin;
         std::string bpm;
         std::string copyright;
         std::string encodeBy;
         std::string mood;
         std::string media;
         std::string label;
         std::string catalogNumber;
         std::string barCode;
         // Sort
         std::string titleSort;
         std::string albumSort;
         std::string artistSort;
         std::string albumArtistSort;
         std::string composerSort;
         // External IDs
         std::string mbTrackId;
         std::string mbAlbumId;
         std::string navidromeId;
         // Methods
         bool areMembersValid();
         void addDefaultSortMembers();
   };

   class Album {
      public:
         std::string albumid;
         std::string album;
         std::string albumArtist;
         std::string trackNumber;
         std::string discNumber;
         std::string date;
         std::string genre;
         std::string composer;
         bool compilation;
         std::string albumArtPath;
   };
   
   class Library {
      public:
         Library(const std::string& dbPath, const std::string& libPath);
         ~Library();
         void refreshLibrary();
      private:
         std::unique_ptr<library::Sql> sql_;
         std::unique_ptr<library::Files> files_;
   };
}
