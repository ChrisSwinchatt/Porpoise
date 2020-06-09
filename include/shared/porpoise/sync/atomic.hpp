#pragma once

namespace porpoise { namespace sync {
    /// Atomic flag.
    struct atomic_flag {
        /// Construct with initial value.
        explicit atomic_flag(bool initial_value = false) : _value(initial_value)
        {}

        /// Set the value and return the previous value.
        bool test_and_set() volatile
        {
            return __sync_bool_compare_and_swap(&_value, false, true);
        }

        /// Clear the value.
        void clear() volatile
        {
            _value = 0;
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

            void store(const_reference value) volatile
            {
                _value = value;
            }

            void store(rvalue_reference value) volatile
            {
                _value = value;
            }

            reference load() volatile
            {
                return _value;
            }

            const_reference load() const volatile
            {
                return _value;
            }

            const_reference compare_and_swap(const_reference value, const_reference next) volatile
            {
                __sync_val_compare_and_swap(&_value, value, next);
            }

            operator reference() volatile
            {
                return _value;
            }

            operator const_reference() const volatile
            {
                return _value;
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
        }

        atomic<value_type>& operator=(const atomic<value_type>& other)
        {
            this->store(other.load());
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
        explicit atomic(const_reference value) { this->_value = value; }                                 \
        explicit atomic(rvalue_reference value) { this->_value = value; }                                \
        TYPE operator++()                                                                                 \
        {                                                                                                 \
            return __sync_fetch_and_add(&_value, 1);                                                     \
        }                                                                                                 \
        TYPE operator--()                                                                                 \
        {                                                                                                 \
            return __sync_fetch_and_sub(&_value, 1);                                                     \
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
            return __sync_fetch_and_add(&(this->_value), 1);
        }

        pointer operator--()
        {
            return __sync_fetch_and_sub(&(this->_value), 1);
        }
    };

}} // porpoise::sync