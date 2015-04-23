#include "fileinfo.h"

namespace http {

//#include <boost/filesystem.hpp>
//#include <boost/filesystem/operations.hpp>
//#include <boost/filesystem/path.hpp>
#include <sys/stat.h>

//inline bool is_directory( file_status f ) { return f.type() == directory_file; }
//inline bool exists( file_status f )       { return f.type() != status_unknown && f.type() != file_not_found; }

bool FileInfo::IsExist(const std::string& path)
{
    struct stat buffer;
    return (stat (path.c_str(), &buffer) == 0);

    //return boost::filesystem::exists(path.toStdString());
}

} // End http.
