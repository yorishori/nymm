// ===============
// Internal Header
// ===============
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <filesystem>
#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>

#include <sqlite3.h>

#include "library.h"

namespace library {
   struct SqliteCloser {void operator()(sqlite3* db) const noexcept {sqlite3_close(db);};};

   class Sql {
      public:
         explicit Sql(const std::filesystem::path& path);

         void initDb();
         void insertTracks(const std::vector<library::Track>& trks);
         void updateAlbums(const std::vector<library::Album>& albs);
      private:
         std::unique_ptr<sqlite3, SqliteCloser> db_;
    };

   class Files {
      public:
         explicit Files(std::filesystem::path path, library::Sql& sql) : musicLibPath_(path), sql_(sql) {}
         void refreshLibrary();
      private:
         std::filesystem::path musicLibPath_;
         library::Sql& sql_;
   };

   template <typename T>
   class Q {
      public:
         explicit Q(size_t capacity) : capacity_(capacity) {}

         void push(T item) {
            std::unique_lock<std::mutex> lock(mtx_);
            notFull_.wait(lock, [&]{ return q_.size() < capacity_ || isDone_; });
            q_.push(std::move(item));
            lock.unlock();
            notEmpty_.notify_one();
         }

         std::vector<T> popBatch(size_t maxN, std::chrono::milliseconds timeout) {
            std::unique_lock<std::mutex> lock(mtx_);
            notEmpty_.wait_for(lock, timeout, [&]{
               return q_.size() >= maxN || isDone_;
            });
            std::vector<T> out;
            while (!q_.empty() && out.size() < maxN) {
               out.push_back(std::move(q_.front()));
               q_.pop();
            }
            lock.unlock();
            notFull_.notify_all();
            return out;
         }

         void finish() {
            { std::lock_guard<std::mutex> lock(mtx_); isDone_ = true; }
            notEmpty_.notify_all();
            notFull_.notify_all();
         }

         bool isQDone() {
            std::lock_guard<std::mutex> lock(mtx_);
            return isDone_ && q_.empty();
         }

      private:
         std::queue<T> q_;
         std::mutex mtx_;
         std::condition_variable notEmpty_;
         std::condition_variable notFull_;
         bool isDone_ = false;
         const size_t capacity_;
   };


   class InvalidDatabaseException : public std::runtime_error {
      public:
         using std::runtime_error::runtime_error;
   };

   class InvalidTrackException : public std::runtime_error {
      public:
         using std::runtime_error::runtime_error;
   };
}
