#pragma once

#ifdef _WIN32
  #ifdef _EXPORTING
    #define API_EXPORT __declspec(dllexport)
  #else
    #define API_EXPORT __declspec(dllimport)
  #endif
#else
  #define API_EXPORT
#endif

// extern "C" {
//   API_EXPORT ...
// }