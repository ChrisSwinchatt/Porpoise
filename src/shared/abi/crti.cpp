#include <stdint.h>

using fn_ptr = void(*)();

extern "C" fn_ptr __init_array_start__[];
extern "C" fn_ptr __init_array_end__[];

extern "C" fn_ptr __fini_array_start__[];
extern "C" fn_ptr __fini_array_end__[];

extern "C" void _init()
{
    auto p = __init_array_start__;
    auto q = __init_array_end__;
    for (; p < q; p++)
    {
        (*p)();
    }
}

extern "C" void _fini()
{
    auto p = __fini_array_start__;
    auto q = __fini_array_end__;
    for (; p < q; p++)
    {
        (*p)();
    }
}

fn_ptr __init_array_start__[] __attribute__((used, section(".init_array"), aligned(sizeof(fn_ptr)))) = {};
fn_ptr __fini_array_start__[] __attribute__((used, section(".fini_array"), aligned(sizeof(fn_ptr)))) = {};
