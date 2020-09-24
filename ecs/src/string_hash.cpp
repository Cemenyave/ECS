#include "../include/string_hash.h"

string_hash_t hash_str(const char* s)
{
  const string_hash_t A = 54059; /* a prime */
  const string_hash_t B = 76963; /* another prime */
  const string_hash_t C = 86969; /* yet another prime */
  const string_hash_t FIRSTH = 37; /* also prime */
  string_hash_t h = FIRSTH;
  while (*s) {
    h = (h * A) ^ (s[0] * B);
    s++;
  }
  return h;
}
