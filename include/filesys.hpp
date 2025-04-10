#ifndef FILESYS_HPP
#define FILESYS_HPP

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
    Metadata(const std::string& _file)
        : filename(ext_filename(_file)),
          bsize(ext_bsize(_file)),
          size(ext_size(_file)),
          dist(ext_dist(_file)),
          id(ext_id(_file))
    {}

private:
    std::smatch& match(const std::string& _file) {
        static const std::regex pattern(R"((?:.*/)?int(\d+)_(\d+)([KMBT])_([a-zA-Z]+)_(\d+).*)");
        static std::smatch match;
        static bool executed = false;
        if (executed) return match;

        if (!std::regex_search(_file, match, pattern))
            throw std::invalid_argument("Cannot extract metadata: " + _file);
        
        executed = true;
        return match;
    }

    std::string ext_filename(const std::string& _file) {
        return match(_file)[0].str();
    }

    std::int16_t ext_bsize(const std::string& _file) {
        return static_cast<std::int16_t>(std::stoi(match(_file)[1].str()));
    }

    std::int64_t ext_size(const std::string& _file) {
        return parse_suffix(match(_file)[2].str() + match(_file)[3].str());
    }

    std::string ext_dist(const std::string& _file) {
        return match(_file)[4].str();
    }

    std::int16_t ext_id(const std::string& _file) {
        return static_cast<std::int16_t>(std::stoi(match(_file)[5].str()));
    }

public:
    const std::string filename;
    const std::int16_t bsize;
    const std::int64_t size;
    const std::string dist;
    const std::int16_t id;
};

class Mount {
public:
    Mount(const std::string& _filename)
        : meta(_filename),
          fin(_filename, std::ios::binary),
          data(meta.size * meta.bsize) {
        if (!std::filesystem::exists(_filename)) throw std::runtime_error("No such a file: " + _filename);
        if (!fin) throw std::runtime_error("Cannot open the file: " + _filename);
        fin.read(reinterpret_cast<char*>(data.data()), data.size());
    }

    template<class T>
    T& at(std::int64_t _index) {
        return *(reinterpret_cast<T*>(data.data()) + _index);
    }

    template<class T>
    const T& at(std::int64_t _index) const {
        return *(reinterpret_cast<const T*>(data.data()) + _index);
    }

    void reset(void) {
        fin.clear();
        fin.seekg(0, std::ios::beg);
        fin.read(reinterpret_cast<char*>(data.data()), data.size());
    }

public:
    Metadata meta;

private:
    std::ifstream fin;
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