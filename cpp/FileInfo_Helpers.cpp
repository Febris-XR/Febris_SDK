// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"

namespace FebrisCppHelpers
{
    class FileInfo
    {
    public:
        FileInfo(const std::filesystem::directory_entry& entry)
        {
            name = entry.path().filename().string();
            size = std::filesystem::file_size(entry.path());
            isDirectory = std::filesystem::is_directory(entry.path());
        }

        std::string name;
        std::uintmax_t size;
        bool isDirectory;
    };

    std::vector<FileInfo> GetFileInfo(const std::string& dirPath)
    {
        std::vector<FileInfo> fileInfo;

        std::filesystem::path path(dirPath);

        if (!std::filesystem::is_directory(path))
        {
            // Throw an exception or return an empty vector to indicate an error
            return fileInfo;
        }

        for (auto& entry : std::filesystem::directory_iterator(path))
        {
            fileInfo.push_back(FileInfo(entry));
        }

        return fileInfo;
    };

};