#pragma once

using string_hash_t = unsigned int;

//hash function from here: https://stackoverflow.com/questions/8317508/hash-function-for-a-string
string_hash_t hash_str(const char* s);

