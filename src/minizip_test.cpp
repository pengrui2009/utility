#include <stdio.h>
#include <vector>
#include "minizip/unzip.h"

int getExtraDataLength(const char *zipFilename, const char *fileName)
{
    unzFile uf = unzOpen(zipFilename);
    if (uf == NULL)
    {
        printf("Error opening %s\n", zipFilename);
        return -1;
    }

    int err = unzLocateFile(uf, fileName, 0);
    if (err != UNZ_OK)
    {
        printf("Error locating %s in %s\n", fileName, zipFilename);
        unzClose(uf);
        return -1;
    }

    unz_file_info file_info;
    char filename[512];
    err = unzGetCurrentFileInfo(uf, &file_info, filename, sizeof(filename), NULL, 0, NULL, 0);
    if (err != UNZ_OK)
    {
        printf("Error getting file info for %s\n", fileName);
        unzClose(uf);
        return -1;
    }

    int extraDataLength = file_info.size_file_extra;
    // int extraDataLength = file_info.; //file_info.extra_length;
    printf("extra data len:%d\n", extraDataLength);

    unzClose(uf);

    return extraDataLength;
}

int main()
{
    const char *zipfile = "ziptest.zip";
    const char *file = "main.cpp";

    int length = getExtraDataLength(zipfile, file);
    if (length >= 0)
    {
        printf("Extra data length for %s in %s: %d\n", file, zipfile, length);
    }

    unzFile unzfile = unzOpen(zipfile);
    if (unzfile == NULL)
    {
        printf("Error opening %s\n", zipfile);
        return -1;
    }

    unz_global_info *pGlobalInfo = new unz_global_info;
    int nReturnValue = unzGetGlobalInfo(unzfile, pGlobalInfo);
    if (nReturnValue != UNZ_OK)
    {
        // 处理获取信息失败的情况
        printf("unzGetGlobalInfo failed.\n");
        return -1;
    }
#define MAX_PATH 200
    unz_file_info *pFileInfo = new unz_file_info;
    char szZipFName[MAX_PATH] = {0};

    // for (int i = 0; i < pGlobalInfo->number_entry; i++)
    int err = unzGoToFirstFile(unzfile);
    while (err == UNZ_OK)
    {
        nReturnValue = unzGetCurrentFileInfo(unzfile, pFileInfo, szZipFName, MAX_PATH, NULL, 0, NULL, 0);
        if (nReturnValue != UNZ_OK)
        {
            // 处理获取文件信息失败的情况
            continue;
        }

        int extraDataLength = pFileInfo->size_file_extra;
        // int extraDataLength = file_info.; //file_info.extra_length;
        printf("filename:%s filename len:%d extra data_len:%d compressed_size:%d\n", szZipFName,
               pFileInfo->size_filename, extraDataLength, pFileInfo->compressed_size);
        // 打开当前文件
        nReturnValue = unzOpenCurrentFile(unzfile);
        if (nReturnValue != UNZ_OK)
        {
            // 处理打开当前文件失败的情况
            continue;
        }
        // 读取文件内容

        std::vector<char> buf(pFileInfo->uncompressed_size + 1);
        int len = unzReadCurrentFile(unzfile, &buf[0], buf.size());
        if (len < 0)
        {
            // std::cerr << "Error reading " << file << std::endl;
            unzCloseCurrentFile(unzfile);
            unzClose(unzfile);
            return 1;
        }
        printf("read buf:%s\n", buf.data());
        // 关闭当前文件
        unzCloseCurrentFile(unzfile);

        err = unzGoToNextFile(unzfile);
    }
    unzClose(unzfile);

    return 0;
}