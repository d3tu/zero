#pragma once

#include <fstream>

namespace Core {
  namespace Embeder {
    struct Info {

    };

    static const char MAGIC[] = {'Z', '3', 'r', '0', 'L', '4', 'n', 'g'};

    long long MAGIC_LEN = sizeof(MAGIC);
    long long INFO_SIZE = sizeof(Info);
    
    const char *encode(Info info) {
      auto bytes = new char[INFO_SIZE + MAGIC_LEN];

      auto p = bytes;

      while (p - bytes < MAGIC_LEN) {
        *p = MAGIC[p - bytes];

        ++p;
      }

      auto infoBytes = reinterpret_cast<char *>(&info);

      while (p - bytes < INFO_SIZE + MAGIC_LEN) {
        *p = infoBytes[p - bytes - MAGIC_LEN];;

        ++p;
      }

      return bytes;
    }

    Info decode(const char *bytes) {
      auto p = bytes;

      while (*p && *p == MAGIC[p - bytes]) {
        ++p;
      }

      if (*(p - 1) != MAGIC[MAGIC_LEN]) {
        throw;
      }

      auto info = new char[INFO_SIZE];

      while (p - bytes < INFO_SIZE) {
        info[p - bytes] = *p;

        ++p;
      }

      return *reinterpret_cast<Info *>(info);
    }

    void writeBinary(const char *path, const char *bytes, std::streamsize length) {
      std::ofstream file(path, std::ios::binary);
      file.write(bytes, length);
      file.close();
    }
  }
}