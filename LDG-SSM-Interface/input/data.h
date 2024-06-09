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
 * @param output_buffer
 * @param file_name
 * @return
 */
template<typename DataType>
long readBZipFile(std::vector<DataType> &output_buffer, std::string file_name)
{
    std::ifstream file_stream(file_name, std::ios::binary);
    if (!file_stream.is_open()) {
        qDebug() << "Could not open file: " << file_name;
        return -1;
    }

    // Find file size
    file_stream.seekg(0, std::ios::end);
    int file_size = file_stream.tellg();
    file_stream.seekg(0, std::ios::beg);

    // Create buffers
    uint output_buf_size = output_buffer.size() * sizeof(DataType) > file_size ? output_buffer.size() * sizeof(DataType) : file_size * 2;
    output_buffer.resize(output_buf_size / sizeof(DataType));
    std::vector<char> input_buffer(file_size);

    // Read data and decompress
    file_stream.read(input_buffer.data(), file_size);
    int bz_error = BZ2_bzBuffToBuffDecompress(
        reinterpret_cast<char *>(output_buffer.data()),
        &output_buf_size,
        input_buffer.data(),
        file_size,
        0,
        0
        );

    if (bz_error != BZ_OK) {
        qDebug() << "Could not decompress file: " << file_name;
        return -1;
    }

    output_buffer.resize(output_buf_size / sizeof(DataType));
    return output_buffer.size();
}

/**
 * @brief readFileIntoBuffer Read data into a buffer using the appropriate function.
 * @param buffer
 * @param file_name
 * @return
 */
template<typename DataType>
long readFileIntoBuffer(std::vector<DataType> &buffer, QString file_name)
{
    if (file_name.endsWith(".bz2")) {
        return readBZipFile(buffer, file_name.toStdString());
    }
    if (file_name.endsWith(".raw")) {
        return readRawFile(buffer, file_name.toStdString());
    }
    return -1;
}

#endif // DATA_H
