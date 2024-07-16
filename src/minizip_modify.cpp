#include <iostream>
#include <fstream>
#include "minizip/unzip.h"
#include "minizip/zip.h"

int main()
{
    // 输入的 Zip 文件
    const char *zipfile = "example.zip";
    // 要修改的文件
    const char *file = "example.txt";
    // 替换文件的路径
    const char *replace_file = "new_example.txt";
    // 输出的新的 Zip 文件
    const char *new_zipfile = "new_example.zip";

    unzFile uf = unzOpen(zipfile);
    if (uf == NULL)
    {
        std::cerr << "Error opening " << zipfile << std::endl;
        return 1;
    }

    zipFile zf = zipOpen(new_zipfile, APPEND_STATUS_CREATE);
    if (zf == NULL)
    {
        std::cerr << "Error creating " << new_zipfile << std::endl;
        unzClose(uf);
        return 1;
    }

    int err = unzGoToFirstFile(uf);
    while (err == UNZ_OK)
    {
        unz_file_info file_info;
        char filename[512];
        err = unzGetCurrentFileInfo(uf, &file_info, filename, sizeof(filename), NULL, 0, NULL, 0);
        if (err != UNZ_OK)
        {
            std::cerr << "Error getting file info" << std::endl;
            unzClose(uf);
            zipClose(zf, NULL);
            return 1;
        }

        if (
            std::string(filename) == file)
        {
            // 打开替换文件
            std::ifstream replace_stream(replace_file, std::ios::binary);
            if (!replace_stream.is_open())
            {
                std::cerr << "Error opening " << replace_file << std::endl;
                unzClose(uf);
                zipClose(zf, NULL);
                return 1;
            }
            // 读取替换文件的数据
            std::vector<char> buf((std::istreambuf_iterator<char>(replace_stream)), std::istreambuf_iterator<char>());

            // 添加新文件到输出的 Zip 文件中
            err = zipOpenNewFileInZip(zf, file, &file_info, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
            if (err != UNZ_OK)
            {
                std::cerr << "Error adding file to " << new_zipfile << std::endl;
                unzClose(uf);
                zipClose(zf, NULL);
                return 1;
            }

            err = zipWriteInFileInZip(zf, &buf[0], buf.size());
            if (err != UNZ_OK)
            {
                std::cerr << "Error writing to " << new_zipfile << std::endl;
                unzClose(uf);
                zipClose(zf, NULL);
                return 1;
            }

            Copy code
                err = zipCloseFileInZip(zf);
            if (err != UNZ_OK)
            {
                std::cerr << "Error closing " << new_zipfile << std::endl;
                unzClose(uf);
                zipClose(zf, NULL);
                return 1;
            }
        }
        else
        {
            // 如果不是要修改的文件，则复制到输出的 Zip 文件中
            err = unzOpenCurrentFile(uf);
            if (err != UNZ_OK)
            {
                std::cerr << "Error opening " << filename << std::endl;
                unzClose(uf);
                zipClose(zf, NULL);
                return 1;
            }

            // 添加新文件到输出的 Zip 文件中
            err = zipOpenNewFileInZip(zf, filename, &file_info, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
            if (err != UNZ_OK)
            {
                std::cerr << "Error adding file to " << new_zipfile << std::endl;
                unzClose(uf);
                zipClose(zf, NULL);
                return 1;
            }

            // 读取文件并复制到输出的 Zip 文件中
        std::vector<char> buf(file_info.uncomp
        err = zipCloseFileInZip(zf);
        if(err != UNZ_OK)
        {
                std::cerr << "Error closing " << new_zipfile << std::endl;
                unzClose(uf);
                zipClose(zf, NULL);
                return 1;
        }
        }
        else
        {
            // 如果不是要修改的文件，则复制到输出的 Zip 文件中
            err = unzOpenCurrentFile(uf);
            if (err != UNZ_OK)
            {
                std::cerr << "Error opening " << filename << std::endl;
                unzClose(uf);
                zipClose(zf, NULL);
                return 1;
            }
            // 添加新文件到输出的 Zip 文件中
            err = zipOpenNewFileInZip(zf, filename, &file_info, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
            if (err != UNZ_OK)
            {
                std::cerr << "Error adding file to " << new_zipfile << std::endl;
                unzClose(uf);
                zipClose(zf, NULL);
                return 1;
            }
            // 读取文件并复制到输出的 Zip 文件中
            std::vector<char> buf(file_info.uncompressed_size);
            int len = unzReadCurrentFile(uf, &buf[0], buf.size());
            if (len < 0)
            {
                std::cerr << "Error reading " << filename << std::endl;
                unzCloseCurrentFile(uf);
                unzClose(uf);
                zipClose(zf, NULL);
                return 1;
            }
            err = zipWriteInFileInZip(zf, &buf[0], buf.size());
            if (err != UNZ_OK)
            {
                std::cerr << "Error writing to " << new_zipfile << std::endl;
                unzClose(uf);
                zipClose(zf, NULL);
                return 1;
            }
            err = zipCloseFileInZip(zf);
            if (err != UNZ_OK)
            {
                std::cerr << "Error closing " << new_zipfile << std::endl;
                unzClose(uf);
                zipClose(zf, NULL);
                return 1;
            }
            err = unzCloseCurrentFile(uf);
            if (err != UNZ_OK)
            {
                std::cerr << "Error closing " << filename << std::endl;
                unzClose(uf);
                zipClose(zf, NULL);
                return 1;
            }
        }
        err = unzGoToNextFile(uf);
    }
    // 关闭
    unzClose(uf);
    // 关闭输出的 Zip 文件
    err = zipClose(zf, NULL);
    if (err != UNZ_OK)
    {
        std::cerr << "Error closing " << new_zipfile << std::endl;
        return 1;
    }
    std::cout << "Successfully replaced " << file << " in " << zipfile << " with " << replace_file << std::endl;
    return 0;
}