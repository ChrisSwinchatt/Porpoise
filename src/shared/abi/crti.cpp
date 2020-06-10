using fn_ptr = void(*)();

extern "C" fn_ptr _init_array_start[];
extern "C" fn_ptr _init_array_end[];

extern "C" fn_ptr _fini_array_start[];
extern "C" fn_ptr _fini_array_end[];

extern "C" void _init(void)
{
    for (auto fn = _init_array_start; fn < _init_array_end; fn++)
    {
        (*fn)();
    }
}

extern "C" void _fini(void)
{
    for (auto fn = _fini_array_start; fn < _fini_array_end; fn++)
    {
        (*fn)();
    }
}

fn_ptr _init_array_start[0] __attribute__((used, section(".init_array"), aligned(sizeof(fn_ptr)))) = {};
fn_ptr _fini_array_start[0] __attribute__((used, section(".fini_array"), aligned(sizeof(fn_ptr)))) = {};
