#pragma once
#include <unordered_map>
#include "hashmap.h"

#ifdef __NO_INLINE__
#define __DEBUG__ 1
#endif

template<class T, class U>
// using hashmap = std::unordered_map<T, U>;
using hashmap = emilib::HashMap<T, U>;

using id = size_t;

#define HPYLM_INITIAL_D 	0.5
#define HPYLM_INITIAL_THETA 2.0
#define HPYLM_INITIAL_A 	1.0
#define HPYLM_INITIAL_B 	1.0
#define HPYLM_INITIAL_ALPHA 1.0
#define HPYLM_INITIAL_BETA  1.0

#define VPYLM_BETA_STOP  4
#define VPYLM_BETA_PASS  1
#define VPYLM_EPS 1e-12

#define ID_BOS 0
#define ID_EOS 1
#define ID_BOW 2
#define ID_EOW 3