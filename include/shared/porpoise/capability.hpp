#pragma once

namespace porpoise {
    // Capabilities are tag structs that callees to some APIs must possess in order to call the API. They can only be
    // instantiated in boot-kernel.cpp, and are used to prevent e.g. code that runs before the heap is initialised from
    // using the heap.

    /// Capability required by initialisation functions.
    struct init_capability;

    /// Capability required by heap functions.
    struct heap_capability;
} // porpoise
