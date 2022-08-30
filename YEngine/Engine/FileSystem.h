#pragma once
#include <vector>
#include <string>
#include <filesystem>

class FileSystem
{
public:
	static bool IsFile(const std::string& path);
	static std::string GetFileExtension(const std::string& filename);
	static std::wstring StringToWide(std::string str);
	static std::string GetDirectoryFromFilePath(const std::string& path);
};

