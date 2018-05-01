#include "FileInfo.hpp"
#include <string>

FileInfo::FileInfo() {
  offset=0;
  size=0;
  dateAdded="";
}

FileInfo::FileInfo(int anOffset, int aSize, std::string aDate) {
  offset=anOffset;
  size=aSize;
  dateAdded=aDate;
}

FileInfo& FileInfo::operator=(const FileInfo& aCopy) {
  offset=aCopy.offset;
  size = aCopy.size;
  dateAdded = aCopy.dateAdded;
  return *this;
}

int FileInfo::getOffset() {
  return offset;
}

void FileInfo::setOffset(int newOffset) {
  offset=newOffset;
}

int FileInfo::getSize() {
  return size;
}

std::string FileInfo::getDateAdded() {
  return dateAdded;
}
