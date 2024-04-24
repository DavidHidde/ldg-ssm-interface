#ifndef DATA_H
#define DATA_H

#include <QDebug>
#include <cstddef>
#include <fstream>

/**
 * @brief readRawFile Read data from a .raw file into a buffer
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

#endif // DATA_H
