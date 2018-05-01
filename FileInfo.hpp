// built off of simon's file class
#ifndef FileInfo_hpp
#define FileInfo_hpp

#include <string>


class FileInfo {
public:
    FileInfo();

    FileInfo(int anOffset, int aSize, std::string aDate);

    FileInfo& operator=(const FileInfo& aCopy);
    
    int getOffset();
    int getSize();
    std::string getDateAdded();
    void setOffset(int newOffset);

private:
    int offset;
    int size;
    std::string dateAdded;
};

#endif
