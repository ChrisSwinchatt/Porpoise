#include <stdint.h>

using fn_ptr = void(*)();

extern "C" fn_ptr _init_array_start[];
extern "C" fn_ptr _init_array_end[];

extern "C" fn_ptr _fini_array_start[];
extern "C" fn_ptr _fini_array_end[];

extern "C" void _init()
{
    auto p = reinterpret_cast<uintptr_t>(_init_array_start);
    auto q = reinterpret_cast<uintptr_t>(_init_array_end);
    for (; p < q; p++)
    {
        (*reinterpret_cast<fn_ptr>(p))();
    }
}

extern "C" void _fini()
{
    auto p = reinterpret_cast<uintptr_t>(_fini_array_start);
    auto q = reinterpret_cast<uintptr_t>(_fini_array_end);
    for (; p < q; p++)
    {
        (*reinterpret_cast<fn_ptr>(p))();
    }
}

fn_ptr _init_array_start[] __attribute__((used, section(".init_array"), aligned(sizeof(fn_ptr)))) = {};
fn_ptr _fini_array_start[] __attribute__((used, section(".fini_array"), aligned(sizeof(fn_ptr)))) = {};
