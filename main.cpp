#include "StorageEngine.hpp"
#include <iostream>
#include <map>



void addFile(char* arch, char* file)
{
    StorageEngine myEngine(arch);
    myEngine.addFile(file);
    
}

void deleteFile(char* arch, char* file)
{
    StorageEngine myEngine(arch);
    myEngine.deleteFile(file);
    std::cout << "File '" << file << "' deleted from archive '" << arch << "'" << std::endl;
}

void listFile(char* arch, char* file)
{
    StorageEngine myEngine(arch);
    myEngine.listArchive(file);
    
    //std::cout << "File '" << file << "' Properties - bla bla" << std::endl;
}

void listArchive(char* arch)
{
    StorageEngine myEngine(arch);
    myEngine.listArchive();
    //std::cout << "Archive - " << arch << "' Files in archive - " << std::endl;
}

void findFile(char* arch, char* file)
{
    StorageEngine myEngine(arch);
    myEngine.findFile(file);
}

void extractFile(char* arch, char* file)
{
    StorageEngine myEngine(arch);
    myEngine.extractFile(file);
    std::cout << "File '" << file << "' extracted." << std::endl;
}


int main(int argc, char* argv[])
{
    static std::map<std::string, int> mapInt;
    mapInt["-v"] = 6;
    mapInt["-version"] = 6;
    mapInt["version"] = 6;
    mapInt["add"] = 1;
    mapInt["del"] = 2;
    mapInt["delete"] = 2;
    mapInt["list"] = 3;
    mapInt["-l"] = 3;
    mapInt["find"] = 4;
    mapInt["extract"] = 5;
    mapInt["test"] = 10;    // For tests
    std::string initial = "./sfarchiver";
    //std::cout << argv[0] << std::endl;
    if(argc == 1){
      std::cout << "Invalid command!" << std::endl;
      return 0;
    }
    
    if(true)
    {
        switch(mapInt[argv[1]])
        {
            case 6:
                if(argc == 2)
                {
                    std::cout << "sfarchiver version 1.0 Feb 27, 2018" << std::endl;
                }
                else
                    std::cout << "Invalid command!" << std::endl;
                break;

            case 1:
                if(4 == argc)
                {
                    char* archiveName = argv[2];
                    char* fileName = argv[3];
                    addFile(archiveName, fileName);
                }
                else
                    std::cout << "Invalid command!" << std::endl;
                break;

            case 2:
                if(4 == argc)
                {
                    char* archiveName = argv[2];
                    char* fileName = argv[3];
                    deleteFile(archiveName, fileName);
                }
                else
                    std::cout << "Invalid command!" << std::endl;
                break;

            case 3:
                if(4 == argc)
                {
                    char* archiveName = argv[2];
                    char* fileName = argv[3];
                    listFile(archiveName, fileName);
                }
                else if(3 == argc)
                {
                    char* archiveName = argv[2];
                    listArchive(archiveName);
                }
                else
                    std::cout << "Invalid command!" << std::endl;
                break;

            case 4:
                if(4 == argc)
                {
                    char* archiveName = argv[2];
                    char* fileName = argv[3];
                    findFile(archiveName, fileName);
                }
                else
                    std::cout << "Invalid command!" << std::endl;
                break;

            case 5:
                if(4 == argc)
                {
                    char* archiveName = argv[2];
                    char* fileName = argv[3];
                    extractFile(archiveName, fileName);
                }
                else
                    std::cout << "Invalid command!" << std::endl;
                break;

            default:
                std::cout << "Invalid command!" << std::endl;

        }
    }
    

    return 0;
}
