#pragma once

#include <fstream>

namespace DJ {
  namespace Embed {
    struct Info {
      std::streamsize data_pos;
    };

    const char MAGIC[] = {'d', 'j', 'l', '4', 'n', 'g', '!', '!'};
    long long int MAGIC_SIZE = sizeof(MAGIC) / sizeof(*MAGIC);
    long long int INFO_SIZE = sizeof(Info) + MAGIC_SIZE;

    struct Data {
      std::streamsize size;
      char *data;

      operator char *() const {
        return data;
      }
    };

    void setWindowsBinaryToGui(char *bin, size_t size) {
      if (size < 64) throw;
      unsigned int start_pe = *reinterpret_cast<const unsigned int *>(&bin[60]);
      unsigned short start_32 = start_pe + 28;
      unsigned short magic_32 = *reinterpret_cast<const unsigned short *>(&bin[start_32]);
      unsigned short start_64 = start_pe + 24;
      unsigned short magic_64 = *reinterpret_cast<const unsigned short *>(&bin[start_64]);
      int standard_fields_size = magic_32 == 0x10b ? 28 : magic_64 == 0x20b ? 24 : throw;
      int subsystem_offset = 68;
      int subsystem_start = start_pe + standard_fields_size + subsystem_offset;
      unsigned short subsystem = 2;
      memcpy(&bin[subsystem_start], &subsystem, sizeof(subsystem));
    }

    bool isEmbed(const char *path) {
      std::ifstream file(path, std::ios::binary | std::ios::ate);
      file.seekg(-INFO_SIZE, std::ios::end);
      auto bytes = new char[INFO_SIZE];
      file.read(bytes, INFO_SIZE);
      file.close();
      auto p = bytes;
      while (p - bytes < INFO_SIZE && MAGIC[p - bytes] == *p) ++p;
      auto result = *(p - 1) == MAGIC[p - bytes - 1];
      delete[] bytes;
      return result;
    }

    char *encodeInfo(Info info) {
      auto data = new char[INFO_SIZE];
      auto p = data;
      while (p - data < MAGIC_SIZE) {
        *p = MAGIC[p - data];
        ++p;
      }
      auto bytes = reinterpret_cast<char *>(&info);
      while (p - data < INFO_SIZE) {
        *p = bytes[p - data - MAGIC_SIZE];
        ++p;
      }
      return data;
    }

    Info *decodeInfo(char *bytes) {
      auto p = bytes;
      while (p - bytes < INFO_SIZE && MAGIC[p - bytes] == *p) ++p;
      if (*(p - 1) != MAGIC[p - bytes - 1]) throw;
      auto data = new char[INFO_SIZE - MAGIC_SIZE];
      while (p - bytes < INFO_SIZE) {
        data[p - bytes - MAGIC_SIZE] = *p;
        ++p;
      }
      return reinterpret_cast<Info *>(data);
    }

    Data readBinary(const char *path) {
      std::ifstream file(path, std::ios::binary | std::ios::ate);
      if (!file.is_open()) throw;
      auto size = file.tellg();
      file.seekg(0, std::ios::beg);
      auto data = new char[size];
      if (!file.read(data, size)) throw;
      file.close();
      return { size, data };
    }

    void encodeBinary(const char *input, const char *output, const char *data, int size, bool no_terminal = false) {
      std::ofstream file(output, std::ios::binary);
      auto executable = readBinary(input);
      #ifdef _WIN32
        if (no_terminal) setWindowsBinaryToGui(executable.data, executable.size);
      #endif
      file.write(executable.data, executable.size);
      file.write(data, size);
      file.write(encodeInfo({ executable.size }), INFO_SIZE);
      file.close();
    }

    Data decodeBinary(const char *path) {
      std::ifstream file(path, std::ios::binary | std::ios::ate);
      file.seekg(-INFO_SIZE, std::ios::end);
      auto bytes = new char[INFO_SIZE];
      std::streamsize pos = file.tellg();
      file.read(bytes, INFO_SIZE);
      auto info = decodeInfo(bytes);
      delete[] bytes;
      file.seekg(info->data_pos, std::ios::beg);
      auto size = pos - info->data_pos;
      auto data = new char[size];
      file.read(data, size);
      file.close();
      return { size, data };
    }
  }
}