#ifndef StorageEngine_hpp
#define StorageEngine_hpp

#include <string>
#include <fstream>
#include "FileInfo.hpp"
#include <map>

class StorageEngine {
public:
  StorageEngine(const char* archiveName = nullptr);
  ~StorageEngine();

  void addFile(const char* file);
  void deleteFile(const char* file);
  void listFile(const char* file) const;
  void listArchive() const;
  void listArchive(const char* file) const;
  void findFile(const char* file_content);
  void extractFile(const char* file);

private:
  std::fstream data; 
  std::string archiveExt;
  int nFiles;
  int headerLength;
  std::map<std::string, FileInfo> filenames;
  bool deleted = false;
  int deleted_offset = 0;
  int deleted_length = 0;

};

#endif
