#include "FileSystem.h"
#include <Macros/macros.h>

bool FileSystem::IsFile(const std::string& path)
{
    if (path.empty())
        return false;

    try
    {
        if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path))
            return true;
    }
    catch (std::filesystem::filesystem_error& e)
    {
        LOG_ERROR("%s, %s", e.what(), path.c_str());
    }

    return false;
}

std::string FileSystem::GetFileExtension(const std::string& filename)
{
    size_t off = filename.find_last_of('.');
    if (off == std::string::npos)
    {
        return {};
    }
    return std::string(filename.substr(off + 1));
}
std::wstring FileSystem::StringToWide(std::string str)
{
    std::wstring wide_string(str.begin(), str.end());
    return wide_string;
}

std::string FileSystem::GetDirectoryFromFilePath(const std::string& path)
{
    const size_t last_index = path.find_last_of("\\/");

    if (last_index != std::string::npos)
        return path.substr(0, last_index + 1);

    return "";
}

