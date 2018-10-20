#pragma once


#define LIKELY(c) __builtin_expect(c, 1)
#define UNLIKELY(c) __builtin_expect(c, 0)
