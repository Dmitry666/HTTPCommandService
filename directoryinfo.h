#ifndef HTTP_DIRECTORYINFO_H
#define HTTP_DIRECTORYINFO_H

#include "fileinfo.h"

namespace openrc {

class DirectoryInfo
{
public:
    DirectoryInfo(const std::string& path);
    ~DirectoryInfo();

    bool IsExist();
    bool Create();

    std::vector<FileInfo> GetFiles() const;

    static std::vector<FileInfo> GetFiles(const std::string& path)
    {
        return DirectoryInfo(path).GetFiles();
    }

    static bool CreatePath(const std::string& path);
    static bool IsExist(const std::string& path);

protected:
    std::string _path;
};

} // End http.

#endif // DIRECTORYINFO_H
