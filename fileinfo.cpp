#include "fileinfo.h"

namespace openrc {

//#include <boost/filesystem.hpp>
//#include <boost/filesystem/operations.hpp>
//#include <boost/filesystem/path.hpp>
#include <sys/stat.h>
//#include <unistd.h>
#include <time.h>
//inline bool is_directory( file_status f ) { return f.type() == directory_file; }
//inline bool exists( file_status f )       { return f.type() != status_unknown && f.type() != file_not_found; }

time_t FileInfo::LastModifiedTime(const std::string& path)
{
	struct stat attrib;
	if(stat(path.c_str(), &attrib))
	{
		return 0;
	}

	struct tm* clock = gmtime(&(attrib.st_mtime));
	return mktime(clock);
}

bool FileInfo::IsExist(const std::string& path)
{
    struct stat buffer;
    return (stat (path.c_str(), &buffer) == 0);

    //return boost::filesystem::exists(path.toStdString());
}

} // End http.
