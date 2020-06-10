#pragma once

namespace porpoise { namespace sync {
    enum class memory_order
    {
        seq_cst = __ATOMIC_SEQ_CST,
        acq_rel = __ATOMIC_ACQ_REL,
        release = __ATOMIC_RELEASE,
        acquire = __ATOMIC_ACQUIRE,
        consume = __ATOMIC_CONSUME,
        relaxed = __ATOMIC_RELAXED
    };

    /// Atomic flag.
    struct atomic_flag {
        /// Construct with initial value.
        explicit atomic_flag(bool initial_value = false) : _value(initial_value)
        {}

        /// Set the value and return the previous value.
        bool test_and_set(memory_order order = memory_order::seq_cst) volatile
        {
            return __atomic_test_and_set(&_value, static_cast<int>(order));
        }

        /// Clear the value.
        void clear(memory_order order = memory_order::seq_cst) volatile
        {
            __atomic_clear(&_value, static_cast<int>(order));
        }
    private:
        bool _value;
    };

    namespace internal {
        template <class T>
        struct atomic_base {
            using value_type       = T;
            using reference        = volatile T&;
            using const_reference  = const volatile T&;
            using rvalue_reference = T&&;

            void store(const_reference next, memory_order order = memory_order::seq_cst) volatile
            {
                __atomic_store(&_value, &next, static_cast<int>(order));
            }

            void store(rvalue_reference next, memory_order order = memory_order::seq_cst) volatile
            {
                __atomic_store(&_value, &next, static_cast<int>(order));
            }

            value_type load(memory_order order = memory_order::seq_cst) const volatile
            {
                value_type loaded;
                __atomic_load(&_value, &loaded, static_cast<int>(order));
                return loaded;
            }

            value_type compare_and_swap(
                const_reference expect,
                const_reference next,
                memory_order    order = memory_order::seq_cst) volatile
            {
                value_type tmp = expect;
                __atomic_compare_exchange(&_value, &tmp, &next, false, static_cast<int>(order));
                return tmp;
            }

            operator value_type() volatile
            {
                return load();
            }
        protected:
            value_type _value;
        };
    }

    /// Atomic wrapper.
    template <class T>
    struct atomic : public ::porpoise::sync::internal::atomic_base<T>
    {
        using value_type       = typename ::porpoise::sync::internal::atomic_base<T>::value_type;
        using reference        = typename ::porpoise::sync::internal::atomic_base<T>::reference;
        using const_reference  = typename ::porpoise::sync::internal::atomic_base<T>::const_reference;
        using rvalue_reference = typename ::porpoise::sync::internal::atomic_base<T>::rvalue_reference;

        explicit atomic(const_reference value) { this->_value = value; }

        explicit atomic(rvalue_reference value) { this->_value = value; }

        atomic<value_type>& operator=(const value_type& other)
        {
            this->store(other);
            return *this;
        }

        atomic<value_type>& operator=(const atomic<value_type>& other)
        {
            this->store(other.load());
            return *this;
        }
    };

#define NUMERIC_LIST(F)     \
    F(char)                 \
    F(signed char)          \
    F(signed int)           \
    F(signed long)          \
    F(signed long long)     \
    F(unsigned char)        \
    F(unsigned int)         \
    F(unsigned long)        \
    F(unsigned long long)

#define ATOMIC_SPEC(TYPE)                                                                                 \
    template <>                                                                                           \
    struct atomic<TYPE> : public ::porpoise::sync::internal::atomic_base<TYPE>                            \
    {                                                                                                     \
        using value_type       = typename ::porpoise::sync::internal::atomic_base<TYPE>::value_type;      \
        using reference        = typename ::porpoise::sync::internal::atomic_base<TYPE>::reference;       \
        using const_reference  = typename ::porpoise::sync::internal::atomic_base<TYPE>::const_reference; \
        using rvalue_reference = typename ::porpoise::sync::internal::atomic_base<TYPE>::rvalue_reference;\
        explicit atomic(const_reference value) { this->_value = value; }                                  \
        explicit atomic(rvalue_reference value) { this->_value = value; }                                 \
        TYPE operator++()                                                                                 \
        {                                                                                                 \
            return __atomic_fetch_add(&_value, 1, static_cast<int>(memory_order::seq_cst));               \
        }                                                                                                 \
        TYPE operator--()                                                                                 \
        {                                                                                                 \
            return __atomic_fetch_sub(&_value, 1, static_cast<int>(memory_order::seq_cst));               \
        }                                                                                                 \
        TYPE operator++(int)                                                                              \
        {                                                                                                 \
            return __atomic_add_fetch(&_value, 1, static_cast<int>(memory_order::seq_cst));               \
        }                                                                                                 \
        TYPE operator--(int)                                                                              \
        {                                                                                                 \
            return __atomic_sub_fetch(&_value, 1, static_cast<int>(memory_order::seq_cst));               \
        }                                                                                                 \
    };

    NUMERIC_LIST(ATOMIC_SPEC)

#undef NUMERIC_LIST
#undef ATOMIC_SPEC

    template <class T>
    struct atomic<T*> : public ::porpoise::sync::internal::atomic_base<T*>
    {
        using value_type       = T;
        using pointer          = T*;
        using const_pointer    = const T*;
        using reference        = T&;
        using const_reference  = const T&;
        using rvalue_reference = T&&;

        explicit atomic(pointer value) { this->_value = value; }

        const_reference operator*() const
        {
            return *(this->_value);
        }

        reference operator*()
        {
            return *(this->_value);
        }

        const_pointer operator->() const
        {
            return (this->_value);
        }

        pointer operator->()
        {
            return (this->_value);
        }

        pointer operator++()
        {
            return __atomic_fetch_add(&(this->_value), 1, static_cast<int>(memory_order::seq_cst));
        }

        pointer operator--()
        {
            return __atomic_fetch_sub(&(this->_value), 1, static_cast<int>(memory_order::seq_cst));
        }

        pointer operator++(int)
        {
            return __atomic_add_fetch(&(this->_value), 1, static_cast<int>(memory_order::seq_cst));
        }

        pointer operator--(int)
        {
            return __atomic_sub_fetch(&(this->_value), 1, static_cast<int>(memory_order::seq_cst));
        }
    };

}} // porpoise::sync