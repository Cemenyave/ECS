#pragma once
#include "../include/string_hash.h"
#include <iostream>

#ifdef WIN32
#define STRCPY strcpy_s
#else
#define STRCPY strcpy

#endif

class SimpleString {
  enum : uint32_t { string_size = 256 };
  char str[string_size] = "";
public:
  SimpleString() = default;
  SimpleString(const char *s)
  {
    assert(strlen(s) < string_size - 1);
    STRCPY(str, s);
  }

  operator const char* () const { return str; }
  bool operator== (const SimpleString& other) {
    return hash_str(str) == hash_str(other.str);
  }
};
