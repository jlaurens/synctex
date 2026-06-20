/**
 * Reader_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */


#include <iostream>
#include <memory>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#define SYNCTEX_SKIP * doctest::skip()

TEST_SUITE_BEGIN("POC_0");

TEST_CASE("[POC] move the buffer") {
    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7};
    // Déplacer la plage [3, 4, 5] (indices 2 à 5) vers la position 0
    // vec devient : {3, 4, 5, 1, 2, 6, 7}
    std::move(vec.begin() + 2, vec.end(), vec.begin());
    vec.resize(vec.size()-2);
    std::cout << vec.size() << std::endl;
    for (const auto& element : vec) {
        std::cout << element << " ";
    }
    std::cout << std::endl << "DONE [POC] move the buffer" << std::endl;
    CHECK(1==1);
}

TEST_SUITE_END;

#define SYNCTEX_TEST_BUFFER

namespace POC {

    struct BufferChef {
    public:
    struct Size {
        static constexpr size_t float10 = 1 + // 1 for the separator
            std::numeric_limits<float>::max_digits10
                + std::numeric_limits<float>::max_exponent10
        ;
        static constexpr size_t min = float10;
        static constexpr size_t max = 2<<19; // 1Mo
        static constexpr size_t buffer = std::max(min, (size_t)std::max(PATH_MAX, 2<<12));
    };
    private:
    public:
    size_t _capacity;
    size_t _size;
    size_t _position;
    std::unique_ptr<char[]> _data;

    public:
    BufferChef(): BufferChef(0) {}
    BufferChef(size_t capacity):
        _capacity(0),
        _size(0),
        _position(0)
    {
        // this->capacity(1);
        // reset(capacity);
    }
    size_t capacity(size_t capacity = 0)
    {
        if (capacity > 0) {
            _capacity = Size::max;
            _capacity = std::max(
                Size::min,
                std::min(capacity, Size::max)
            );
        }
        return _capacity;
    };
    size_t size(size_t size = 0)
    {
        return _size = std::min(_capacity, size);
    };
    bool reset(size_t capacity = 0) {
        this->capacity(capacity);
        // _data.reset(new char(_capacity));
        return false;
    };
    #ifdef SYNCTEX_TEST_BUFFER
    void fill() {
        auto p = _data.get();
        for (size_t i = 0; i < _size; ++i) {
            p[i] = char((static_cast<int>('0')+i)%10);
        }
        for (size_t i = _size; i < _capacity; ++i) {
            p[i] = char((static_cast<int>('A')+i-_size)%10);
        }
    }
    std::string string(size_t max = 0) {
        auto p = _data.get();
        if (p) {
            auto size = _size - _position;
            if (0 < max && max < size) size = max;
            return std::string(p+_position, size);
        }
        return "";
    }
    std::string whole_string(size_t max = 0) {
        auto p = _data.get();
        if (p) {
            auto size = _capacity;
            if (0 < max && max < size) size = max;
            return std::string(p, size);
        }
        return "";
    }
    #endif
};

};

TEST_SUITE_BEGIN("POC" SYNCTEX_SKIP);

TEST_CASE("[POC] static") {
    CHECK(POC::BufferChef::Size::min == 48);
    std::cout << std::endl << "DONE [POC] static" << std::endl;
}

TEST_CASE("[POC] BufferChef()" SYNCTEX_SKIP) {
    auto buffer = POC::BufferChef();
    // std::cout << "buffer._capacity " <<buffer._capacity << std::endl;
    // auto capacity = buffer._capacity;
    // CHECK(capacity == 0);
    // CHECK(buffer._capacity == 0);
    // CHECK(buffer._size == 0);
    // CHECK(buffer._position == 0);
    std::cout << std::endl << "DONE [POC] BufferChef()" << std::endl;
}

TEST_CASE("[POC] character buffer") {
    size_t _size = 10;
    size_t _capacity = 20;
    std::unique_ptr<char[]> _buffer(new char(_capacity));
    auto p = _buffer.get();
    CHECK(p);
    for (size_t i = 0; i < _size; ++i) {
        p[i] = char(static_cast<int>('0')+i);
    }
    for (size_t i = _size; i < _capacity; ++i) {
        p[i] = char(static_cast<int>('A')+i-_size);
    }
    std::string s(p, _capacity);
    CHECK(s == "0123456789ABCDEFGHIJ");
}

TEST_SUITE_END();

