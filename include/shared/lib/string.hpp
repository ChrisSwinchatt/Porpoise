#pragma once

#include <stdint.h>
#include <stddef.h>

extern "C" size_t strlen(const char* s);

extern "C" void* memcpy(void* dst, const void* src, size_t n);

extern "C" void* memmove(void* dst, const void* src, size_t n);
