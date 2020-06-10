using fn_ptr = void(*)();

fn_ptr _init_array_end[0] __attribute__((used, section(".init_array"), aligned(sizeof(fn_ptr)))) = {};
fn_ptr _fini_array_end[0] __attribute__((used, section(".fini_array"), aligned(sizeof(fn_ptr)))) = {};
