#pragma once

template <class T>
struct comparer {
    virtual int operator()(const T& a, const T& b) const = 0;
};


template <class T>
struct default_comparer : comparer<T> {
    int operator()(const T& a, const T& b) const override
    {
        if (a < b)
        {
            return -1;
        }

        if (a > b)
        {
            return 1;
        }

        return 0;
    }
};
