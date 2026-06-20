/**
 * BufferChef_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */


#include <iostream>
#include <memory>

#include "SyncTeXBufferChef.hpp"

using namespace SyncTeXpp;

class TestBufferChef0: public BufferChef
{
    int fill(char *p, int len) override {
        return 0;
    };
    public:
    // TestBufferChef0(): BufferChef() {}
    TestBufferChef0(int capacity = 0, bool force = false): BufferChef(capacity, true) {}
    Restore_f save() override {
        return [](){};
    };
};

class TestBufferChef: public BufferChef
{

    public:
    int _max = 20; // change the test if you change this
    TestBufferChef(int capacity): BufferChef(capacity) {}

    void test_fill() {
        auto p = mutable_data();
        if (p) {
            const int p6n = position();
            for (int i = 0 ; i < p6n ; ++i) {
                p[i] = static_cast<int>('a')+i%10;
            }
            const int s2e = size();
            for (int i = p6n ; i < s2e ; ++i) {
                p[i] = static_cast<int>('0')+(i-p6n)%10;
            }
            for (int i = s2e; i < capacity(); ++i) {
                p[i] = static_cast<int>('A')+(i-s2e)%10;
            }
        }
    }

    std::string test_string(int position = 0, int max = 0) {
        int len = 0;
        auto p = data(len, position);
        if (p) {
            if (0 < max && max < len) len = max;
            if (len) return std::string(p, len);
        }
        return "⟨NONE⟩";
    }

    std::string test_whole_string(int max = 0) {
        const char *p = mutable_data();
        if (p && capacity()) {
            int len = capacity();
            if (0 < max && max < len) len = max;
            return std::string(p, len);
        }
        return "⟨NONE⟩";
    }
    int fill(char *p, int len) override {
        // std::cout << "##### reading: " << std::endl;
        // std::cout << "len: " << len << std::endl;
        // std::cout << "BEFORE p: " <<  std::string(p, len) << std::endl;
        int read = len > _max ? _max : len;
        for (auto i = 0 ; i < read ; ++i) {
            p[i] = int('a')+ i%10;
        }
        // std::cout << "AFTER  p: " <<  std::string(p, len) << std::endl;
        return read;
    };

    Restore_f save() override {
        return [](){};
    };

};
 
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

using namespace SyncTeXpp;

TEST_SUITE("[BufferChef]") {

TEST_CASE("BufferChef()") {
    auto chef = TestBufferChef0(421, true);
    CHECK(chef.capacity() == 421);
}

TEST_CASE("BufferChef(421)") {
    auto chef = TestBufferChef0(421);
    CHECK(chef.capacity() == 421);
}

TEST_CASE("BufferChef()+reset(...)") {
    auto chef = TestBufferChef0();
    chef.reset();
    CHECK(chef.capacity() == BufferChef::Size::capacity);
}

TEST_CASE("BufferChef()+reset(...)") {
    auto chef = TestBufferChef0();
    chef.reset(2 * BufferChef::Size::min);
    CHECK(chef.capacity() == 2 * BufferChef::Size::min);
}

TEST_CASE("BufferChef()+size(...)") {
    auto s = 100;
    auto chef = TestBufferChef0(s, true);
    CHECK(chef.capacity() == s);
    CHECK(chef.size() == 0);
    chef.enlarge(s/2);
    CHECK(chef.size() == s/2);
    chef.enlarge(s/2);
    CHECK(chef.size() == s);
    chef.enlarge(-s/2);
    CHECK(chef.size() == s/2);
    chef.enlarge(s);
    CHECK(chef.size() == s);
    chef.enlarge(-s);
    CHECK(chef.size() == 0);
    chef.enlarge(2*s);
    CHECK(chef.size() == s);
    chef.enlarge(-2*s);
    CHECK(chef.size() == 0);
}

TEST_CASE("BufferChef()") {
    auto s = 4 * BufferChef::Size::min;
    auto chef = TestBufferChef0(4 * s);
    CHECK(chef.capacity() == 4 * s);
    chef.enlarge(s/2);
    CHECK(chef.position() == 0);
    chef.advance(0);
    CHECK(chef.position() == 0);
    chef.advance(-100);
    CHECK(chef.position() == 0);
    chef.advance(s/4);
    CHECK(chef.position() == s/4);
    chef.advance(1);
    CHECK(chef.position() == s/4+1);
    chef.advance(-2);
    CHECK(chef.position() == s/4-1);
    chef.advance(s);
    CHECK(chef.position() == s / 2);
}

TEST_CASE("TestBufferChef()") {
    auto chef = TestBufferChef(60);
    CHECK(chef.capacity() == 60);
    chef.enlarge(30);
    chef.advance(10);
    chef.test_fill();
    std::cout << chef.test_whole_string() << std::endl;
    CHECK(chef.test_whole_string() == "abcdefghij01234567890123456789ABCDEFGHIJABCDEFGHIJABCDEFGHIJ");
    CHECK(chef.test_string() == "01234567890123456789");
    CHECK(chef.test_string(-5) == "fghij01234567890123456789");
    CHECK(chef[3] == '3');
    CHECK(chef[-1] == '9');
    chef.shift();
    CHECK(chef.test_string() == "01234567890123456789");
    CHECK(chef.test_whole_string() == "012345678901234567890123456789ABCDEFGHIJABCDEFGHIJABCDEFGHIJ");
    CHECK(chef[3] == '3');
    CHECK(chef[-1] == '9');
}

TEST_CASE("BufferChef expect") {
    auto chef = TestBufferChef0(60);
    CHECK(chef.capacity() == 60);
    chef.enlarge(30);
    CHECK(Status::Done== chef.expect(29) );
    CHECK(Status::Done== chef.expect(30) );
    CHECK(Status::Done <  chef.expect(40));
    CHECK(Status::Done <  chef.expect(41));
    chef.advance(15);
    CHECK(Status::Done== chef.expect(14) );
    CHECK(Status::Done== chef.expect(15) );
    CHECK(Status::Done <  chef.expect(16));
    CHECK(Status::Done <  chef.expect(17));
    chef.advance(15);
    CHECK(Status::Done== chef.expect( 0) );
    CHECK(Status::Done <  chef.expect( 1));
    CHECK(Status::Done <  chef.expect( 2));
}

TEST_CASE("TestBufferChef expect") {
    auto chef = TestBufferChef(60);
    CHECK(chef.capacity() == 60);
    chef.test_fill();
    CHECK(chef.test_whole_string() == "ABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ");
    CHECK(Status::Done <  chef.expect( 60 ));
    CHECK(chef.test_whole_string() == "abcdefghijabcdefghijABCDEFGHIJABCDEFGHIJABCDEFGHIJABCDEFGHIJ");
    CHECK(Status::Done == chef.expect( 20 ) );
    CHECK(Status::Done <  chef.expect( 60 ));
    CHECK(chef.test_whole_string() == "abcdefghijabcdefghijabcdefghijabcdefghijABCDEFGHIJABCDEFGHIJ");
    CHECK(Status::Done == chef.expect( 40 ) );
    CHECK(Status::Done == chef.expect( 60 ) );
    CHECK(chef.test_whole_string() == "abcdefghijabcdefghijabcdefghijabcdefghijabcdefghijabcdefghij");
    CHECK(0 == chef.position());
    CHECK(60 == chef.size());
    CHECK(60 == chef.capacity());
    CHECK(Status::Done <  chef.expect( 61 ));
}

}

TEST_SUITE_END();

