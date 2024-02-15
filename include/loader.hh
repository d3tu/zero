#pragma once

#ifdef _WIN32
  #include <windows.h>
  typedef HMODULE LibHandle;
  typedef FARPROC FuncPtr;
  #ifdef UNICODE
    #define LOAD_LIBRARY(name) LoadLibraryW((LPCWSTR) name)
  #else
    #define LOAD_LIBRARY(name) LoadLibraryA(name)
  #endif
  #define GET_PROC_ADDRESS(handle, procName) GetProcAddress(handle, procName)
  #define FREE_LIBRARY(handle) FreeLibrary(handle)
#else
  #include <dlfcn.h>
  typedef void *LibHandle;
  typedef void *FuncPtr;
  #define LOAD_LIBRARY(name) dlopen(name, RTLD_LAZY)
  #define GET_PROC_ADDRESS(handle, procName) dlsym(handle, procName)
  #define FREE_LIBRARY(handle) dlclose(handle)
#endif