#include "directoryinfo.h"


#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

//inline bool is_directory( file_status f ) { return f.type() == directory_file; }
//inline bool exists( file_status f )       { return f.type() != status_unknown && f.type() != file_not_found; }

namespace openrc {

DirectoryInfo :: DirectoryInfo(const std::string& path)
    : _path(path)
{}

DirectoryInfo :: ~DirectoryInfo()
{}

bool DirectoryInfo :: IsExist()
{
    return true; //return boost::filesystem::exists(_path);
}

bool DirectoryInfo :: Create()
{
#if 0
    namespace fs = boost::filesystem;

    fs::path dir(_path);
    try
    {
        fs::create_directories(dir);
    }
    catch(std::exception& e)
    {
        printf("Create directory '%s'. Exception %s", _path.c_str(), e.what());
        return false;
    }
#endif
    return true;
}

std::vector<FileInfo> DirectoryInfo::GetFiles() const
{
    std::vector<FileInfo> files;

//#if 0
    fs::path someDir(_path.c_str());
    fs::directory_iterator end_iter;

    if (fs::exists(someDir) && fs::is_directory(someDir))
    {
        for (fs::directory_iterator dir_iter(someDir); dir_iter != end_iter; ++dir_iter)
        {
            if (fs::is_regular_file(dir_iter->status()))
            {
                std::string path = dir_iter->path().string();
                files.push_back(FileInfo(path.c_str()));
            }
        }
    }
//#endif
    return files;
}

bool DirectoryInfo :: CreatePath(const std::string& path)
{
#if 0
    namespace fs = boost::filesystem;
    fs::path dir(path);

    try
    {
        fs::create_directories(dir);
    }
    catch(std::exception& e)
    {
        printf("Create directory '%s'. Exception %s", path.c_str(), e.what());
        return false;
    }
#endif
    return true;
}

bool DirectoryInfo::IsExist(const std::string& directoryPath)
{
#if 0
    return boost::filesystem::exists(directoryPath);
#endif
    return true;
}

} // End http.
