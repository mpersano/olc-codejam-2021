#include "util.h"

#include <fstream>

std::vector<uint8_t> readFile(const char *path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to load file " + std::string(path) + "?");

    auto *buf = file.rdbuf();

    const std::size_t size = buf->pubseekoff(0, file.end, file.in);
    buf->pubseekpos(0, file.in);

    std::vector<uint8_t> data(size);
    buf->sgetn(reinterpret_cast<char *>(data.data()), size);

    file.close();

    return data;
}
