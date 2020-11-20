#pragma once

#include <stdint.h>
#include <stddef.h>

#include <container/array.hpp>
#include <container/bitarray.hpp>
#include <container/sorted-vector.hpp>
#include <memory.hpp>

#include <porpoise/capability.hpp>
#include <porpoise/sync/spinlock.hpp>

#ifndef CONFIG_HEAP_BLOCKS_PER_BIN
# error CONFIG_HEAP_BLOCKS_PER_BIN is not defined
#endif

#ifndef CONFIG_HEAP_NUM_FIXED_BIN
# error CONFIG_HEAP_NUM_FIXED_BIN is not defined
#endif

#ifndef CONFIG_HEAP_MIN_BIN_BLOCK_SIZE
# error CONFIG_HEAP_MIN_BIN_BLOCK_SIZE is not defined
#endif

#ifndef CONFIG_HEAP_MAX_BIN_BLOCK_SIZE
# error CONFIG_HEAP_MAX_BIN_BLOCK_SIZE is not defined
#endif

#ifndef CONFIG_HEAP_MAX_TOTAL_SIZE
# error CONFIG_HEAP_MAX_TOTAL_SIZE is note defined
#endif

namespace porpoise { namespace mem {
    enum class oom_behaviour
    {
        abort,
        return_null,
        default_ = abort
    };

    bool is_address_aligned(uintptr_t address, size_t boundary);

    uintptr_t get_aligned_address(uintptr_t address, size_t boundary);

    ptrdiff_t make_address_aligned(uintptr_t& address, size_t boundary);

    /// Kernel heap manager.
    struct heap
    {
        static constexpr void* OUT_OF_MEMORY = nullptr;
        static constexpr void* MOVE_BIN = nullptr;
        static constexpr size_t BLOCKS_PER_BIN = CONFIG_HEAP_BLOCKS_PER_BIN;
        static constexpr size_t NUM_FIXED_BIN = CONFIG_HEAP_NUM_FIXED_BIN;
        static constexpr size_t MIN_BIN_BLOCK_SIZE = CONFIG_HEAP_MIN_BIN_BLOCK_SIZE;
        static constexpr size_t MAX_BIN_BLOCK_SIZE = CONFIG_HEAP_MAX_BIN_BLOCK_SIZE;
        static constexpr size_t MAX_HEAP_SIZE = CONFIG_HEAP_MAX_TOTAL_SIZE;
        static constexpr size_t TOTAL_NUM_BIN = NUM_FIXED_BIN + 1;

        /// Object containing statistics about the heap.
        struct statistics {
            size_t num_bins;         ///< The number of bins.
            size_t allocations;      ///< The number of calls to heap::allocate().
            size_t frees;            ///< The number of calls to heap::deallocate().
            size_t total_bytes;      ///< The total number of bytes of memory managed by the heap.
            size_t bytes_allocated;  ///< The number of bytes allocated in the heap.
            size_t total_blocks;     ///< The total number of blocks managed by the heap.
            size_t blocks_allocated; ///< The total number of blocks allocated in the heap.
        };

        /// Get a reference to the heap.
        static heap& get(const heap_capability& cap);

        /// Initialise the heap at address 'start'.
        void init(const init_capability& cap, uintptr_t start);

        /// Map an immovable piece of memory.
        void map(void* p, size_t size);

        /// Allocate a memory region. If memory cannot be allocated, abort or return null depending on oom_behaviour.
        void* allocate(size_t bytes, oom_behaviour behaviour = oom_behaviour::default_);

        /// Reallocate a memory region.
        /// Returns p if p was resized in-place or a new pointer if p was moved. If bytes is 0, p is deallocated and NULL
        /// is returned. If p cannot be resized due to memory constraints, abort is called or NULL is returned depending
        /// on oom_behaviour.
        void* reallocate(void* p, size_t bytes, oom_behaviour behaviour = oom_behaviour::default_);

        /// Deallocate memory region.
        void deallocate(void* p);

        /// Compute heap statistics.
        void get_statistics(statistics& stats);
    private:
        struct heap_bin {
            virtual bool is_variable_heap() const = 0;

            virtual void map(void* p, size_t bytes) = 0;

            virtual bool owns(void* p) const = 0;

            virtual void* allocate(size_t bytes) = 0;

            virtual void* reallocate(void* p, size_t bytes) = 0;

            virtual void deallocate(void* p) = 0;

            virtual size_t total_size() const = 0;

            virtual size_t block_size() const = 0;

            virtual size_t total_blocks() const = 0;

            virtual size_t bytes_allocated() const = 0;

            virtual size_t blocks_allocated() const = 0;
        };

        struct fixed_heap_bin : heap_bin {
            fixed_heap_bin(uintptr_t start, size_t blksize);

            bool is_variable_heap() const override
            {
                return false;
            }

            void map(void* p, size_t bytes) override;

            bool owns(void* p) const override;

            void* allocate(size_t bytes) override;

            void* reallocate(void* p, size_t bytes) override;

            void deallocate(void* p) override;

            size_t total_size() const override;

            size_t block_size() const override;

            size_t total_blocks() const override;

            size_t bytes_allocated() const override;

            size_t blocks_allocated() const override;
        private:
            size_t _allocd;
            bitarray<BLOCKS_PER_BIN> _bitmap;
            size_t _blksize;
            size_t _first_free;
            uintptr_t _start;
        };

        struct variable_heap_bin : heap_bin {
            variable_heap_bin(uintptr_t start, size_t max_size);

            bool is_variable_heap() const override
            {
                return true;
            }

            void map(void* p, size_t bytes) override;

            bool owns(void* p) const override;

            void* allocate(size_t bytes) override;

            void* reallocate(void* p, size_t bytes) override;

            void deallocate(void* p) override;

            size_t total_size() const override;

            size_t block_size() const override;

            size_t total_blocks() const override;

            size_t blocks_allocated() const override;

            size_t bytes_allocated() const override;
        private:
            static constexpr unsigned OFFSET_BITS = 32;
            static constexpr unsigned SIZE_BITS = 30;
            static constexpr uint8_t HEAD_MAGIC = 0xA1;
            static constexpr uint8_t FOOT_MAGIC = 0x2B;

            enum class block_type {
                hole = 0,
                present = 1,
                immutable = 2
            };

            struct block
            {
                uint8_t head;
                unsigned offset : OFFSET_BITS;
                unsigned size : SIZE_BITS;
                block_type type : 2;
                uint8_t foot;

                bool is_valid()
                {
                    return head == HEAD_MAGIC && foot == FOOT_MAGIC;
                }
            } __attribute__((packed));

            using blockp_t = block*;
            struct ascending_block_size_comparer : comparer<blockp_t>
            {
                int operator()(const blockp_t& a, const blockp_t& b) const override;
            };

            struct ascending_block_addr_comparer : comparer<blockp_t>
            {
                int operator()(const blockp_t& a, const blockp_t& b) const override;
            };

            uintptr_t _start;
            size_t _allocd;
            size_t _max_size;

            // Bin which stores blocks.
            fixed_heap_bin _block_bin;

            // Pointers to blocks ordered by free|occupied, then by ascending size.
            sorted_vector<block*, abort_allocator<block*>, ascending_block_size_comparer> _blocks_by_size;

            // Pointers to blocks ordered by 
            sorted_vector<block*, abort_allocator<block*>, ascending_block_size_comparer> _blocks_by_addr;

            block* add_block(uint32_t offset, uint32_t size, block_type type);

            void remove_block(block* b);

            void merge_free_blocks();

            void* get_ptr(block* b);
        };

        static heap _inst;
        array<heap_bin*, TOTAL_NUM_BIN> _bins;
        sync::spinlock _lock;
        uintptr_t _start;
        uintptr_t _end;
        size_t _allocations;
        size_t _frees;

        heap() = default;

        void* handle_oom(oom_behaviour behaviour);
    };
}} // porpoise::mem
