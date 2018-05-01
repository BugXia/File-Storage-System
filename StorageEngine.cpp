#include "StorageEngine.hpp"
#include "FileInfo.hpp"
#include <iostream>
#include <fstream>
#include <string.h>
#include <exception>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iomanip>
#include <ctime>

StorageEngine::StorageEngine(const char* archiveName) {
  if (archiveName == nullptr) {
    throw std::invalid_argument("no archive defined");
  }
  
  archiveExt = archiveName;
  archiveExt += ".sfa";

  
  //std::cout << "opening archive: " << archiveExt << std::endl;
  
  // check if making a new archive or creating a new one
  std::ifstream archive(archiveExt, std::ios::in | std::ios::binary); // try opening the archive
  if (archive.good()) { // archive exists. now we need to extract the data
    // open our temporary file for storing binary data
    data.open(archiveExt+"d", std::ios::out | std::ios::binary);

    //std::cout << "archive successfully opened. extracting data" << std::endl;

    std::string headerdata;
    std::getline(archive,headerdata); // store the data about the header in a string

    // parse string for the length of the header and # of files in this archive
    size_t n = headerdata.find(" "); // string is space delimited
    //headerLength = std::stoi(headerdata.substr(0,n));
    //nFiles = std::stoi(headerdata.substr(n+1));
    nFiles = std::stoi(headerdata);

    //std::cout << "headerdata: " << headerdata << std::endl;
    //std::cout << "header length: " << headerLength;
    //std::cout << " num files: " << nFiles << std::endl;

    // now construct our filenames data structure from the existing data
    for (int i = 0; i < nFiles; i++) {
      std::string filedata;
      std::getline(archive,filedata); // store the data about the current file in a string

      //std::cout << "extracting file data: " << filedata << std::endl;
      
      // get filename
      n = filedata.find(" ");
      std::string filename = filedata.substr(0,n);

      // get filesize
      n = filedata.find(" ", n+1);
      int size = std::stoi(filedata.substr(filename.size()+1,n)); 
      
      // get offset
      size_t m = filedata.find(" ", n+1);
      int offset = std::stoi(filedata.substr(n+1,m));

      // get date
      std::string date = filedata.substr(m+1);
      
      //std::cout << "filename: " << filename << " size: " << size << " offset: " << offset;
      //std::cout << " date: " << date << std::endl;

      FileInfo myInfo(offset, size, date);

      filenames.insert(std::pair<std::string, FileInfo>(filename, myInfo));
    }
    
    // now copy the binaries once we're done with that
    headerLength = archive.tellg();
    //std::cout << "archive stream pos: " << archive.tellg() << std::endl;
    archive.seekg(0,archive.end);
    //std::cout << "archive stream pos: " << archive.tellg() << std::endl;
    int archiveEnd = archive.tellg(); 
    archive.seekg(headerLength);
    //std::cout << "archive stream pos: " << archive.tellg() << std::endl;
    while(archive.tellg()<archiveEnd) {
      data.put(archive.get());
    }

    data.close(); // this creates the sfad file that we need
    // data stream constructed
    //std::cout << "dumping binary for our data file:" << std::endl;
    //system("hexdump asdf.sfad");
    data.open(archiveExt+"d", std::ios::in | std::ios::app | std::ios::binary);
  } else { // archive doesn't exist, we'll create a new one
    data.open(archiveExt+"d", std::ios::out | std::ios::binary);
    //std::cout << "archive not found... creating new archive" << std::endl;
    nFiles = 0;
    headerLength = 0;
  }

}

StorageEngine::~StorageEngine() {
  // close the stream before we can alter the data inside of it
  data.close();

  // custom destructor
  
  //std::cout << "creating header" << std::endl;

  // create header and close file
  std::ofstream tempHeader(archiveExt+"h", std::ios::out | std::ios::binary);
  
  // first add number of files
  tempHeader << std::to_string(nFiles) << std::endl;

  // add file metadata
  for (auto iterator = filenames.begin(); iterator != filenames.end(); ++iterator) {
    std::string metadata;
    // get the filename
    metadata += iterator->first;
    metadata += " ";

    FileInfo filedata = iterator->second;

    // get the file size
    metadata += std::to_string(filedata.getSize());
    metadata += " ";

    // get the offset
    metadata += std::to_string(filedata.getOffset());
    metadata += " ";

    // get the date
    metadata += filedata.getDateAdded();

    // write the metadata to the file
    //std::cout << "writing to header: " << metadata << std::endl;
    tempHeader << metadata << std::endl;
  }  

  tempHeader.close(); // header is complete
  
  //std::cout << "header written!" << std::endl;
  //std::cout << "copying binaries" << std::endl;

  // append data to the header and rename to our .sfa format
  std::ofstream finalFile(archiveExt, std::ios::out | std::ios::binary);
  std::ifstream header(archiveExt+"h");
  std::ifstream filedata(archiveExt+"d");

  // get the length of the header
  //header.seekg(0,header.end);
  //int length = header.tellg();
  //header.seekg(0,header.beg);
  
  //finalFile << std::to_string(length) << " ";
  
  // We are not deleting
  if(!deleted){
    // Tony Implementation
    finalFile << header.rdbuf() << filedata.rdbuf();
    // END Tony Implementation
  
  // We are deleting
  } else if(deleted) {
    finalFile << header.rdbuf();
    filedata.seekg(0,filedata.end);
    
    int dataEnd = filedata.tellg();
    
    filedata.seekg(0,filedata.beg);
    
    
    while(filedata.tellg() < deleted_offset) {
      finalFile.put(filedata.get());
    }
    
    int curr = filedata.tellg();
        
    filedata.seekg(curr + deleted_length);
    
    while(filedata.tellg() < dataEnd) {
      finalFile.put(filedata.get());
    }

    
  }
  

  //std::cout << "binaries copied!" << std::endl;

  // close streams
  finalFile.close();
  header.close();
  filedata.close();

  // delete extra files
  std::remove((archiveExt+"h").c_str());
  std::remove((archiveExt+"d").c_str());

  // free any unused memory. watch out for dangling pointers!

  //std::cout << "closing archive" << std::endl;
}

void StorageEngine::addFile(const char* file) {
  // NEED TO CHECK FOR CASE IN WHICH FILE ALREADY EXISTS IN ARCHIVE -Tony
  // We will give them an error message @Piazza -Simon
  // https://piazza.com/class/jcbdud70lrj7hx?cid=177
  
  // Handles case when filename exists already.
  if(filenames.find(std::string(file)) != filenames.end()){
    std::cout << "ERROR: Cannot add file. "
              << "File with name '"
              << file
              << "' already exists!"
              << std::endl;
    return;
  }
  
  //std::cout << "adding a file: " << file << std::endl;

  std::ifstream input(file, std::ios::in | std::ios::binary); // open the file
  if (!input.good()) { // check if we successfully opened the file
    throw std::invalid_argument("file could not be opened");
  }

  //std::cout << "file successfully opened" << std::endl;
  
  // Go to end of temp data file.
  data.seekg(0, data.end);
  
  // get the offset of this file
  int offset = data.tellp();

  // std::cout << "offset in our archive: " << offset << std::endl;

  // get the file size
  input.seekg(0,input.end);
  int size = input.tellg();
  input.seekg(0,input.beg);

 // std::cout << "filesize: " << size << " bytes." << std::endl;
  //std::cout << "copying file" << std::endl;
  
  data << input.rdbuf();

  // done with this file
  input.close();

  // get the date we added this file
  std::time_t result = std::time(nullptr);
  std::tm* timeData = std::localtime(&result);
  std::string date ="";
  date+=std::to_string(timeData->tm_mon+1);
  date+="/";
  date+=std::to_string(timeData->tm_mday);
  date+="/";
  date+=std::to_string(timeData->tm_year+1900);

  // update parameters for the header
  nFiles++;
  FileInfo myInfo(offset, size, date);

  filenames.insert(std::pair<std::string, FileInfo>(std::string(file), myInfo));
  
  std::cout << "File '" << file << "' added to archive" << std::endl;
  //std::cout << "File '" << file << "' added to archive '" << "'" << std::endl;
}


void StorageEngine::extractFile(const char* file) {
  //std::cout << "extracting a file: " << file << std::endl;
  
  // first check if it exists in our map
  if (!filenames.count(std::string(file))) {
    throw std::invalid_argument("file not found in archive");  
  }

  std::string extractedFile(file);
  //extractedFile+=".sfae";

  // create the filestream
  std::ofstream output(extractedFile, std::ios::out | std::ios::binary);

  // get the offset and length of the specified file
  FileInfo filedata = filenames[std::string(file)];
  int length = filedata.getSize();
  int offset = filedata.getOffset();

  //std::cout << "file length: " << length << " file offset: " << offset << std::endl;

  // move file cursor to the offset
  data.seekg(offset);
  

  // copy the contents
  for (int i = 0; i < length; i++) {
    auto some_byte = data.get();
    output.put(some_byte);
    
  }
}

// Lists all files in the archive.
void StorageEngine::listArchive() const {
  std::cout << "Listing archive:" << std::endl;
  if (!filenames.empty()) {
    for (auto iterator = filenames.begin(); iterator != filenames.end(); ++iterator) {
      std::string filename = iterator->first;
      FileInfo filedata = iterator->second; // always size, offset
      std::cout << filename;
      std::cout << " " << filedata.getSize() << " " << filedata.getDateAdded() << std::endl;
    }
  }
}

// Lists the data for the file matching names.
void StorageEngine::listArchive(const char* file) const {
  std::cout << "Listing archive..." << std::endl;
  if (!filenames.empty()) {
    for (auto iterator = filenames.begin(); iterator != filenames.end(); ++iterator) {
      std::string filename = iterator->first;
      if(filename != file){
        continue;
      }
      FileInfo filedata = iterator->second; // always size, offset
      std::cout << filename;
      std::cout << " " << filedata.getSize() << " " << filedata.getDateAdded() << std::endl;
    }
  }
}


// Deletes a file from the archive.
void StorageEngine::deleteFile(const char* file) {
    if (filenames.empty()){
        return;
    }
    deleted = true;
    nFiles --;
    FileInfo filedata = filenames[std::string(file)];
    deleted_length = filedata.getSize();
    deleted_offset = filedata.getOffset();
    filenames.erase(std::string(file));
    
    for (auto iterator = filenames.begin(); iterator != filenames.end(); ++iterator) {
      int offset = iterator->second.getOffset(); // always size, offset
      if(offset < deleted_offset){
        continue;
      }
      iterator->second.setOffset(offset-deleted_length);
    }
    
    
}

// Prints out file properties for files that contain the given string.
void StorageEngine::findFile(const char* file_content){
    // Close temp data file.
    data.close();
    
    // Open temp data file as input.
    std::ifstream filedata_temp(archiveExt+"d");
    
    // Get length of content string.
    
    int content_length = std::strlen(file_content);
    
    // Tracks if we found the first file with content.
    bool first = true;;
    
    // TODO: Check through only .txt files. Currently searching through all types of files.
    
    // TODO: Print out file type along with size. Currently only printing out size.
    
    // Check through content of each file for the given string.
    for (auto iterator = filenames.begin(); iterator != filenames.end(); ++iterator) {
      std::string filename = iterator->first;
      
      // Check if it is a .txt file.
      std::string txt_extension(".txt");
      std::size_t found_txt = filename.find(txt_extension);
      
      // If it isn't a .txt file continue to next file.
      if(found_txt == std::string::npos){
        continue;
      }
      
      FileInfo filedata = iterator->second; // always size, offset
      
      int size = filedata.getSize();
      int offset = filedata.getOffset();
      
      filedata_temp.seekg(offset);
      
      bool found = false;
      
      
      // Check every byte in the file.
      while(filedata_temp.tellg() < (offset+size)){
        if(found){
          if(first){
            std::cout << "Found files with content: '"
              << file_content
              << "'"
              << std::endl;
            first = false;
          }
          std::cout << filename;
          std::cout << " " << filedata.getSize() << " " << filedata.getDateAdded() << std::endl;
          break;
        }
        int current_pos = filedata_temp.tellg();
        
        // Check the a sequence of bytes matches.
        for(int i = 0; (i < content_length) && (filedata_temp.tellg() < (offset+size)); i++){
          if(file_content[i] != filedata_temp.get()){
            break;
          }
          if(i == (content_length-1)){
            found = true;
          }
        }
        filedata_temp.seekg(current_pos+1);
      }
      
    }
    
    // Write that we didn't find anything with content.
    if(first){
        std::cout << "No text files found with content: '"
                  << file_content
                  << "'"
                  << std::endl;
    }
    
    // Close temp data file.
    filedata_temp.close();
    
    
}
