#ifndef DATASET_HPP
#define DATASET_HPP

#include <fstream>
#include <filesystem>
#include <string>
#include <regex>
#include <vector>
#include <cstdint>
#include <exception>

constexpr std::int64_t STREAM_BLOCK_SIZE = 8192;

std::int64_t parse_suffix(const std::string&);
std::string resolve_duplicated(const std::filesystem::path&, const std::string&);

template<class T>
class Stream {
public:
    Stream(const std::string& _filename)
        : fout(_filename, std::ios::binary),
          max_cnt(STREAM_BLOCK_SIZE / sizeof(T)),
          block(max_cnt) {
        if (!fout) throw std::runtime_error("Cannot open the file: " + _filename);
    }
    ~Stream()
    { this->flush(); fout.close(); }

    Stream& operator<<(const T& _data) {
        block[cnt++] = _data;
        if (cnt >= max_cnt) [[unlikely]] this->flush();
        return *this;
    }

    void flush() {
        fout.write(reinterpret_cast<const char*>(block.data()), cnt * sizeof(T));
        cnt = 0;
    }

private:
    std::ofstream fout;
    const std::int64_t max_cnt;
    std::int64_t cnt = 0;
    std::vector<T> block;
};

class Metadata {
public:
    Metadata(const std::string& _file) {
        std::regex pattern(R"(?:.*/)?int(\d+)_(\d+)([KMBT])_([a-zA-Z]+)_(\d+)$)");
        std::smatch match;

        if (!std::regex_match(_file, match, pattern))
            throw std::invalid_argument("Cannot extract metadata: " + _file);
        
        filename = match[0].str();
        bsize    = static_cast<decltype(bsize)>(std::stoi(match[1].str()));
        size     = parse_suffix(match[2].str() + match[3].str());
        dist     = match[4].str();
        id       = static_cast<decltype(id)>(std::stoi(match[5].str()));
    }

public:
    std::string filename;
    std::int16_t bsize;
    std::int64_t size;
    std::string dist;
    std::int16_t id;
};

class Mount {
public:
    Mount(const std::string& _filename)
        : fin(_filename, std::ios::binary),
          meta(_filename),
          data(meta.size * meta.bsize) {
        if (!fin) throw std::runtime_error("Cannot open the file: " + _filename);
        fin.read(reinterpret_cast<char*>(data.data()), data.size());
        fin.close();
    }

    template<class T>
    T at(std::size_t _index) const {
        return *(reinterpret_cast<const T*>(data.data()) + _index);
    }

private:
    std::ifstream fin;
    Metadata meta;
    std::vector<uint8_t> data;
};

std::int64_t parse_suffix(const std::string& str) {
    std::regex pattern(R"(^(\d+)([KMBT]?)$)");
    std::smatch match;

    if (!std::regex_match(str, match, pattern))
        throw std::invalid_argument("Invalid format: " + str);
    
    std::int64_t number = std::stoll(match[1].str());
    switch (std::toupper(match[2].str()[0])) {
    case 'T': number <<= 10; [[fallthrough]];
    case 'B': number <<= 10; [[fallthrough]];
    case 'M': number <<= 10; [[fallthrough]];
    case 'K': number <<= 10;
    }
    return number;
}

std::string resolve_duplicated(const std::filesystem::path& path, const std::string& base) {
    std::int16_t cnt = 0;
    auto format = [&](std::int16_t n) { return base + "_" + std::to_string(n); };
    while (std::filesystem::exists(path / (format(++cnt) + ".sorted")));
    return format(cnt);
}

#endif