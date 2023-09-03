#pragma once
#include <cassert>
#include <chrono>
#include <thread>
#include <type_traits>

#define SDecayOf(t) std::decay_t<decltype(t)>
#define SNameOf(x) #x

#ifdef NDEBUG
#define SAssert(expr)
#define SStaticAssert(expr, msg)
#else
#define SAssert(expr) assert(expr)
#define SStaticAssert(expr, msg) static_assert(expr, msg)
#endif

#define SSleep(_ms) std::this_thread::sleep_for(std::chrono::milliseconds(_ms));