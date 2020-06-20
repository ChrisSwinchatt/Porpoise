#include <stdint.h>
#include <stddef.h>

#include <algorithm.hpp>
#include <container/sorted-vector.hpp>
#include <container/vector.hpp>
#include <memory.hpp>
#include <meta-math.hpp>
#include <move.hpp>

#include <porpoise.hpp>
#include <porpoise/abort.hpp>
#include <porpoise/mem/heap.hpp>

extern uint8_t __kernel_end__[];

namespace porpoise { namespace mem {
    int heap::variable_heap_bin::ascending_block_size_comparer::operator()(const blockp_t& a, const blockp_t& b) const
    {
        if (a->type == block_type::hole && b->type != block_type::hole)
        {
            return 1;
        }
        else if (b->type == block_type::hole && a->type != block_type::hole)
        {
            return -1;
        }

        return static_cast<int>(a->size - b->size);
    }

    int heap::variable_heap_bin::ascending_block_addr_comparer::operator()(const blockp_t& a, const blockp_t& b) const
    {
        return static_cast<int>(a->offset - b->offset);
    }

    heap::variable_heap_bin::variable_heap_bin(uintptr_t start, size_t max_size)
    : _start(start + 2*sizeof(block*)*heap::BLOCKS_PER_BIN + sizeof(block)*heap::BLOCKS_PER_BIN)
    , _allocd(0)
    , _max_size(max_size)
    , _block_bin(start + 2*sizeof(block*)*heap::BLOCKS_PER_BIN, sizeof(block))
    , _blocks_by_size(reinterpret_cast<block**>(start), 0, heap::BLOCKS_PER_BIN)
    , _blocks_by_addr(reinterpret_cast<block**>(start + reinterpret_cast<block**>(start)), 0, heap::BLOCKS_PER_BIN)
    {
        add_block(0, _max_size, block_type::hole);
    }

    heap::variable_heap_bin::variable_heap_bin::block* heap::variable_heap_bin::add_block(uint32_t offset, uint32_t size, block_type type)
    {
        auto b = reinterpret_cast<block*>(_block_bin.allocate(sizeof(block)));
        if (b == nullptr)
        {
            return nullptr;
        }

        b->head = HEAD_MAGIC;
        b->offset = offset;
        b->size = size;
        b->type = type;
        b->foot = FOOT_MAGIC;

        _blocks_by_size.add(b);
        _blocks_by_addr.add(b);
        return b;
    }

    void heap::variable_heap_bin::remove_block(block* b)
    {
        PORPOISE_ASSERT_UNEQUAL(static_cast<int>(b->type), static_cast<int>(block_type::hole));
        b->head = 0xBA;
        b->offset = 0xDEADC0DE;
        b->size = 0x1BADB10C;
        b->foot = 0xAD;
        _blocks_by_size.remove(b);
        _blocks_by_addr.remove(b);
        _block_bin.deallocate(b);
    }

    void heap::variable_heap_bin::merge_free_blocks()
    {
        bool merged;
        do
        {
            // Find the first free block. If it is followed by another free block, extend the first block to encompass
            // the second, remove the encompassed second block, and break out to the outer (do/while) loop. The outer
            // loop ends when no adjacent pairs of free blocks are found.
            //
            // It would be nicer to merge adjacent blocks for as long as possible, kepts in a vector, and remove them
            // all in a single pass at the end, but we are constrained by a small stack size and inability to use the
            // heap from within the heap.
            merged = false;
            block* b = nullptr;
            for (auto it = _blocks_by_addr.begin(); it < _blocks_by_addr.end(); it++)
            {
                if ((*it)->type == block_type::present)
                {
                    b = nullptr;
                    continue;
                }

                if (b == nullptr)
                {
                    b = *it;
                }
                else
                {
                    b->size += (*it)->size;
                    remove_block(*it);
                    merged = true;
                    break;
                }
            }
        } while (merged);
    }

    void heap::variable_heap_bin::map(void* p, size_t bytes)
    {
        PORPOISE_ASSERT_UNEQUAL(p, nullptr);
        PORPOISE_ASSERT_GREATER(bytes, 0UL);
        if (!owns(p))
        {
            return;
        }

        merge_free_blocks();

        auto map_start = MAX(_start, reinterpret_cast<uintptr_t>(p));
        auto map_end = map_start + bytes;
        for (auto it = _blocks_by_addr.begin(); it < _blocks_by_addr.end(); it++)
        {
            const auto block_start = reinterpret_cast<uintptr_t>(_start + (*it)->offset);
            const auto block_end = block_start + static_cast<uintptr_t>((*it)->size);

            // No overlap - ignore.
            if (map_end < block_start || map_start > block_end)
            {
                continue;
            }

            if (map_start <= block_start)
            {
                // If a block is entirely contained within the mapped area, mark is as immutable and update map_start
                // for the next block.
                if (map_end >= block_end)
                {
                    (*it)->type = block_type::immutable;
                    map_start = block_end;
                    continue;
                }

                // If the mapped area overlaps the beginning of the block, create a new block for the end and mark the
                // beginning as immutable.
                auto b = add_block(map_end, block_end - map_end, (*it)->type);
                PORPOISE_CHECK_UNEQUAL(b, nullptr);

                (*it)->size = map_end - block_start;
                (*it)->type = block_type::immutable;
                continue;
            }

            // If the mapped area overlaps the end of the block, create a new immutable block for the end and shrink
            // the beginning.
            if (map_end >= block_end)
            {
                auto b = add_block(map_start, block_end - map_end, block_type::immutable);
                PORPOISE_CHECK_UNEQUAL(b, nullptr);

                (*it)->size = map_start - (*it)->offset;
                continue;
            }

            // Finally, if the mapped area is contained within the block, mark the middle section as immutable and
            // create new blocks for the start and end, unless the new blocks would be smaller than MIN_BIN_BLOCK_SIZE,
            // in which case the entire area is marked immutable.
            auto size_left = map_start - block_start;
            if (size_left >= MIN_BIN_BLOCK_SIZE)
            {
                auto b = add_block((*it)->offset, size_left, (*it)->type);
                PORPOISE_CHECK_UNEQUAL(b, nullptr);
                (*it)->offset = map_start;
                (*it)->size -= size_left;
            }

            auto size_right = block_end - map_end;
            if (size_right >= MIN_BIN_BLOCK_SIZE)
            {
                auto b = add_block(map_end, size_right, (*it)->type);
                PORPOISE_CHECK_UNEQUAL(b, nullptr);
                (*it)->size -= size_right;
            }

            (*it)->type = block_type::immutable;
        }
    }

    bool heap::variable_heap_bin::owns(void* p) const
    {
        return reinterpret_cast<uintptr_t>(p) >= _start && reinterpret_cast<uintptr_t>(p) < _start + _allocd;
    }

    void* heap::variable_heap_bin::allocate(size_t bytes)
    {
        if (bytes >= (1 << SIZE_BITS))
        {
            return OUT_OF_MEMORY;
        }

        // Find the first hole in blocks_by_size with a size of at least bytes, which will be the smallest hole that
        // this allocation can fit into.
        for (auto b : _blocks_by_size) {
            if (b->size >= bytes && b->type == block_type::hole)
            {
                // If the size difference is greater than MIN_BIN_BLOCK_SIZE, create a new hole to the right of b.
                // Otherwise, b will be slightly larger than requested.
                auto delta = b->size - bytes;
                if (delta >= MIN_BIN_BLOCK_SIZE)
                {
                    auto hole = add_block(b->offset + bytes, delta, block_type::hole);
                    if (hole == nullptr)
                    {
                        return OUT_OF_MEMORY;
                    }
                }

                return get_ptr(b);
            }
        }

        // We didn't find any hole large enough.
        return OUT_OF_MEMORY;
    }

    void* heap::variable_heap_bin::get_ptr(block* b)
    {
        return reinterpret_cast<void*>(_start + b->offset);
    }

    void* heap::variable_heap_bin::reallocate(void* p, size_t bytes)
    {
        PORPOISE_CHECK_UNEQUAL(p, nullptr);
        PORPOISE_CHECK_GREATER_OR_EQUAL(reinterpret_cast<uintptr_t>(p), _start);
        PORPOISE_CHECK_LESS_OR_EQUAL(reinterpret_cast<uintptr_t>(p), _start + _max_size);
        if (bytes == 0)
        {
            deallocate(p);
            return nullptr;
        }

        if (bytes >= (1 << SIZE_BITS))
        {
            return OUT_OF_MEMORY;
        }

        auto b = reinterpret_cast<block*>(p);
        PORPOISE_CHECK_EQUAL(b->head, HEAD_MAGIC);
        PORPOISE_CHECK_EQUAL(b->foot, FOOT_MAGIC);
        PORPOISE_CHECK_UNEQUAL(static_cast<int>(b->type), static_cast<int>(block_type::immutable));

        if (bytes == b->size)
        {
            return get_ptr(b);
        }

        // Shrink b and create a hole on the right.
        if (bytes < b->size)
        {
            auto delta = b->size - bytes;
            if (delta < MIN_BIN_BLOCK_SIZE)
            {
                // Don't do anything if the size difference is small.
                return get_ptr(b);
            }

            b->size = bytes;
            _allocd -= delta;
            return add_block(b->size, delta, block_type::hole);
        }

        // We're now going to look for a free block big enough for p. We don't want to look at adjacent blocks, so merge
        // any we can in advance.
        merge_free_blocks();

        // If there is a hole big enough on the right, extend b into it.
        auto delta = bytes - b->size;
        auto pblock = _blocks_by_addr.find(b);
        auto next = pblock;
        next++;
        if (next < _blocks_by_addr.end() 
            && (*next)->is_valid() 
            && (*next)->type == block_type::hole 
            && (*next)->size >= bytes)
        {
            // Consume next if it isn't much larger than the size we need.
            if (delta < MIN_BIN_BLOCK_SIZE)
            {
                b->size += (*next)->size;
                _allocd += (*next)->size;
                remove_block(*next);
                return get_ptr(b);
            }

            // Otherwise shrink next.
            (*next)->offset += delta;
            (*next)->size -= delta;
            b->size += delta;
            _allocd += delta;
            return get_ptr(b);
        }

        // If we get here then we need to move p to a new block.
        return allocate(bytes);
    }

    void heap::variable_heap_bin::deallocate(void* p)
    {
        PORPOISE_CHECK_UNEQUAL(p, nullptr);
        PORPOISE_CHECK_GREATER_OR_EQUAL(reinterpret_cast<uintptr_t>(p), _start);
        PORPOISE_CHECK_LESS_OR_EQUAL(reinterpret_cast<uintptr_t>(p), _start + _max_size);

        auto b = reinterpret_cast<block*>(p);
        PORPOISE_CHECK_EQUAL(b->head, HEAD_MAGIC);
        PORPOISE_CHECK_EQUAL(b->foot, FOOT_MAGIC);
        PORPOISE_CHECK_UNEQUAL(static_cast<int>(b->type), static_cast<int>(block_type::immutable));
        b->type = block_type::hole;
        remove_block(b);
        _allocd -= b->size;
    
        merge_free_blocks();
    }

    size_t heap::variable_heap_bin::total_size() const
    {
        return _max_size;
    }

    size_t heap::variable_heap_bin::total_blocks() const
    {
        return _block_bin.total_blocks();
    }

    size_t heap::variable_heap_bin::block_size() const
    {
        return 1;
    }
    
    size_t heap::variable_heap_bin::bytes_allocated() const
    {
        return _allocd;
    }
    
    size_t heap::variable_heap_bin::blocks_allocated() const
    {
        return _block_bin.blocks_allocated();
    }
}} // porpoise::mem
