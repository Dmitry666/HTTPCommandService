#ifndef HTTP_FILEINFO_H
#define HTTP_FILEINFO_H

#include "common.h"

namespace http {

/**
 * @brief File class.
 */
class FileInfo
{
public:
    FileInfo(const std::string& path)
        : _path(path)
    {}

    FileInfo(const FileInfo& file)
        : _path(file._path)
    {}

    std::string Extenstion()
    {
        int32 sep = _path.rfind('.');
        if(sep == -1)
            return "";

        return std::string(_path.begin() + sep + 1, _path.end());
    }

    int32 Contain(const std::string& text)
    {
        return _path.find(text);
    }

    std::string GetDirectory()
    {
        int32 sep = _path.rfind('/');
#ifdef _WIN32
        int32 s0 = _path.rfind('\\');
        if(s0 > sep)
            sep = s0;
#endif
        if(sep == -1)
            return _path;

        return std::string(_path.begin(), _path.begin() + sep);
    }

    std::string GetName()
    {
        int32 sep = _path.rfind('/');
#ifdef _WIN32
        int32 s0 = _path.rfind('\\');
        if(s0 > sep)
            sep = s0;
#endif
        if(sep == -1)
            return _path;

        return std::string(_path.begin() + sep + 1, _path.end());
    }

    std::string GetShortName()
    {
        int32 sep = _path.rfind('/');
#ifdef _WIN32
        int32 s0 = _path.rfind('\\');
        if(s0 > sep)
            sep = s0;
#endif

        int32 eInd = _path.rfind('.');
        if(sep == -1 || eInd < sep)
            return std::string(_path.begin() + sep + 1, _path.end());

        return std::string(_path.begin() + sep + 1, _path.begin() + eInd);
    }

    bool IsFiltered(const std::string& filter)
    {
        int32 sep = filter.rfind('/');
#ifdef _WIN32
        int32 s0 = filter.rfind('\\');
        if(s0 > sep)
            sep = s0;
#endif

        int32 eInd = filter.rfind('.');
        if(eInd != -1 || eInd > sep)
        {
            std::string filterExtension = std::string(_path.begin() + sep + 1, _path.end());
            if(filterExtension != "*" && filterExtension != Extenstion())
                return false;
        }


        std::string filterName = std::string(_path.begin() + sep + 1, _path.begin() + eInd);
        if(filterName != "*" && filterName != GetShortName())
            return false;

        if(sep != -1 )
        {
            std::string filterPath = std::string(_path.begin() + sep + 1, _path.begin() + eInd);
            if(filterPath != GetDirectory())
                return false;
        }

        return true;
    }

    // Operators.
    FileInfo& operator = (const FileInfo& file)
    {
        _path = file._path;
        return (*this);
    }

    // Inline methods.
    const std::string& GetFullPath() const {return _path;}

    // Static methods.
    static bool IsExist(const std::string& path);

protected:
    std::string _path;
};

} // End http.

#endif // FILEINFO_H
