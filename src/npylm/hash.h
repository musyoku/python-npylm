#pragma once
#include <string>

namespace npylm {
	size_t load_bytes(const char* p, int n);
	size_t shift_mix(size_t v);
	size_t unaligned_load(const char* p);
#if __SIZEOF_SIZE_T__ == 4
	size_t hash_bytes(const void* ptr, size_t len, size_t seed);
#elif __SIZEOF_SIZE_T__ == 8
	size_t hash_bytes(const void* ptr, size_t len);
#endif
	size_t hash_wstring(const std::wstring &str);
	size_t hash_substring_ptr(wchar_t const* ptr, int start, int end);		// endを含む
	size_t hash_substring(const std::wstring &str, int start, int end);		// endを含む
}