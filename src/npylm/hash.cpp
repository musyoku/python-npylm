#include "hash.h"

namespace npylm{
	size_t load_bytes(const char* p, int n){
		size_t result = 0;
		--n;
		do{
			result = (result << 8) + static_cast<unsigned char>(p[n]);
		}while(--n >= 0);
		return result;
	}
	size_t shift_mix(size_t v){
		return v ^ (v >> 47);
	}
	size_t unaligned_load(const char* p){
		size_t result;
		__builtin_memcpy(&result, p, sizeof(result));
		return result;
	}
#if __SIZEOF_SIZE_T__ == 4
	size_t hash_bytes(const void* ptr, size_t len, size_t seed){
		size_t seed = static_cast<size_t>(0xc70f6907UL);
		size_t hash = seed ^ len;
		const char* buf = static_cast<const char*>(ptr);
		while(len >= 4){
			size_t k = unaligned_load(buf);
			k *= m;
			k ^= k >> 24;
			k *= m;
			hash *= m;
			hash ^= k;
			buf += 4;
			len -= 4;
		}
		switch(len){
			case 3:
			hash ^= static_cast<unsigned char>(buf[2]) << 16;
			case 2:
			hash ^= static_cast<unsigned char>(buf[1]) << 8;
			case 1:
			hash ^= static_cast<unsigned char>(buf[0]);
			hash *= m;
		};
		hash ^= hash >> 13;
		hash *= m;
		hash ^= hash >> 15;
		return hash;
	}
#elif __SIZEOF_SIZE_T__ == 8
	size_t hash_bytes(const void* ptr, size_t len){
		size_t seed = static_cast<size_t>(0xc70f6907UL);
		static const size_t mul = (((size_t) 0xc6a4a793UL) << 32UL) + (size_t) 0x5bd1e995UL;
		const char* const buf = static_cast<const char*>(ptr);
		const int len_aligned = len & ~0x7;
		const char* const end = buf + len_aligned;
		size_t hash = seed ^ (len * mul);
		for (const char* p = buf; p != end; p += 8){
			const size_t data = shift_mix(unaligned_load(p) * mul) * mul;
			hash ^= data;
			hash *= mul;
		}
		if ((len & 0x7) != 0){
			const size_t data = load_bytes(end, len & 0x7);
			hash ^= data;
			hash *= mul;
		}
		hash = shift_mix(hash) * mul;
		hash = shift_mix(hash);
		return hash;
	}
#endif
	size_t hash_wstring(const std::wstring &str){
		return hash_bytes(str.data(), str.size() * sizeof(wchar_t));
	}
	size_t hash_substring_ptr(wchar_t const* ptr, int start, int end){
		return hash_bytes(ptr + start, (end - start + 1) * sizeof(wchar_t));
	}
	size_t hash_substring(const std::wstring &str, int start, int end){
		wchar_t const* ptr = str.data();
		return hash_substring_ptr(ptr, start, end);
	}
} // namespace npylm