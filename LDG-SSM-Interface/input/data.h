#ifndef DATA_H
#define DATA_H

#include <bzlib.h>

#include <QDebug>
#include <cstddef>
#include <fstream>

/**
 * @brief readRawFile Read data from a .raw file into a buffer.
 * @param buf
 * @param file_name
 * @return
 */
template<typename DataType>
long readRawFile(std::vector<DataType> &buffer, std::string file_name)
{
    std::ifstream file_stream(file_name.c_str(), std::ios::in|std::ios::binary|std::ios::ate);

    if(!file_stream.is_open()) {
        qDebug() << "Could not open file: " << file_name;
        return -1;
    }

    size_t size = file_stream.tellg();
    size_t num_elements = size / sizeof(DataType);

    file_stream.seekg(0, std::ios::beg);
    buffer.resize(num_elements);
    file_stream.read((char*)&buffer[0], num_elements * sizeof(DataType));

    return buffer.size();
}

/**
 * @brief readBZipFile Read data from a .raw.bz2 file into a buffer.
 * @param buffer
 * @param file_name
 * @return
 */
template<typename DataType>
long readBZipFile(std::vector<DataType> &buffer, std::string file_name)
{
    FILE *file = fopen(file_name.c_str(), "rb");
    if (file == NULL) {
        qDebug() << "Could not open file: " << file_name;
        return -1;
    }
    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    int bz_error;
    BZFILE *bzip_file = BZ2_bzReadOpen(&bz_error, file, 0, 0, NULL, 0);
    if (bz_error != BZ_OK) {
        qDebug() << "Could not open file: " << file_name;
        return -1;
    }

    size_t offset = 0;
    int num_elements_read = 0;
    const size_t elements_per_chunk = 1024;
    while (bz_error == BZ_OK && num_elements_read % sizeof(DataType) == 0)
    {
        buffer.resize(std::max(offset + elements_per_chunk, buffer.size()));
        const int num_elements_read = BZ2_bzRead(&bz_error, bzip_file, &buffer[offset], elements_per_chunk * sizeof(DataType));
        offset += num_elements_read / sizeof(DataType);
    }
    buffer.resize(offset);

    if (bz_error != BZ_STREAM_END || num_elements_read % sizeof(DataType) != 0) {
        qDebug() << "Something went wrong while reading file: " << file_name;
        return -1;
    }

    BZ2_bzReadClose(&bz_error, bzip_file);
    fclose(file);
    return buffer.size();
}

/**
 * @brief readFileIntoBuffer Read data into a buffer using the appropriate function.
 * @param buffer
 * @param file_name
 * @return
 */
template<typename DataType>
bool readFileIntoBuffer(std::vector<DataType> &buffer, QString file_name)
{
    if (file_name.endsWith(".bz2")) {
        return readBZipFile(buffer, file_name.toStdString()) > 0;
    }
    if (file_name.endsWith(".raw")) {
        return readRawFile(buffer, file_name.toStdString()) > 0;
    }
    return false;
}

#endif // DATA_H
