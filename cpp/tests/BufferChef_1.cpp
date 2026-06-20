/**
 * BufferChef_1.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */


#include <iostream>
#include <memory>

#include "SyncTeXBufferChef.hpp"

class TestBufferChef: public SyncTeXpp::BufferChef
{
    protected:
    int fill(char *p, int len) override {
        return 0;
    };
    SyncTeXpp::Restore_f save() override {
        std::cout << "NOTHING TO SAVE";
        return [](){
            std::cout << "NOTHING TO RESTORE";
        };
    }
    public:
    TestBufferChef(int capacity = 0): BufferChef(capacity) {}

    void test_reset(std::string s, bool force=false) {
        reset(s.length(), force);
        enlarge(s.length());
        auto p = mutable_data();
        if (p) {
            for (int i = 0 ; i < size() ; ++i) {
                p[i] = s[i];
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
};
 
class TestBufferChef2: public TestBufferChef
{
    std::string _str = "";
    size_t _where = 0;
    private:
    int fill(char *p, int len) override {
        // std::cout << "##### FILL" << std::endl;
        // std::cout << "len: " << len << std::endl;
        // std::cout << "BEFORE: " << std::string(p, len) << std::endl;
        if (_where >= _str.length()) {
            return 0;
        }
        int filled = std::min(
            _str.length() - _where,
            static_cast<size_t>(len));
        for (int i = 0 ; i < filled ; ++i) {
            p[i] = _str[_where+i];
        }
        // std::cout << "AFTER: " << std::string(p, len) << std::endl;
        _where += filled;
        return filled;
    };
    SyncTeXpp::Restore_f save() override {
        int where = _where - size() + position();
        // std::cout << "SAVED: " << std::string(_str.begin() + where, _str.end()) << std::endl;
        return [this, where](){
            _where = where;
            this->set_size(0);
            // std::cout << "RESTORED TO:" << std::string(_str.begin() + _where, _str.end()) << std::endl;
        };
    }
    public:
    TestBufferChef2(
        int capacity = 0,
        bool force = false,
        const std::string_view & str = "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    ): TestBufferChef(capacity),
    _str(str) {
        if (force) {
            reset(capacity, force);
        }
    }
        
    void test_reset(int capacity = 0, bool force=false) {
        reset(capacity, force);
        _where = 0;
    }
};
 
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

using namespace SyncTeXpp;

TEST_SUITE("[BufferChef]") {

TEST_CASE("BufferChef()") {
    auto chef = TestBufferChef();
    CHECK(chef.capacity() == BufferChef::Size::capacity);
}

TEST_CASE("BufferChef() reset") {
    auto chef = TestBufferChef();
    chef.test_reset("0123456789", true);
    CHECK(chef.capacity() == 10);
    CHECK(chef.test_string() == "0123456789");
}

TEST_CASE("BufferChef() require_endl") {
    auto chef = TestBufferChef();
    chef.test_reset("", false);
    CHECK(Status::EndOfData == chef.require_endl());
    CHECK(chef.test_string() == "⟨NONE⟩");
    chef.test_reset("0123456789", true);
    CHECK(Status::EndOfData == chef.require_endl());
    CHECK(chef.test_string() == "⟨NONE⟩");
    chef.test_reset("\n123456789", true);
    CHECK(chef.test_string() == "\n123456789");
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "123456789");
    chef.test_reset("0\n23456789", true);
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "23456789");
    chef.test_reset("01\n3456789", true);
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "3456789");
    chef.test_reset("012345678\n", true);
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "⟨NONE⟩");
    chef.test_reset("\r123456789", true);
    CHECK(chef.test_string() == "\r123456789");
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "123456789");
    chef.test_reset("0\r23456789", true);
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "23456789");
    chef.test_reset("01\r3456789", true);
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "3456789");
    chef.test_reset("012345678\r", true);
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "⟨NONE⟩");
    chef.test_reset("\r\n23456789", true);
    CHECK(chef.test_string() == "\r\n23456789");
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "23456789");
    chef.test_reset("0\r\n3456789", true);
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "3456789");
    chef.test_reset("01\r\n456789", true);
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "456789");
    chef.test_reset("01234567\r\n", true);
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "⟨NONE⟩");
}

TEST_CASE("BufferChef() require_endl CR+LF") {
    auto chef = TestBufferChef2(10, true, "012345678\r" "\nABCDEFGHI" "?????");
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "ABCDEFGHI");
}


TEST_CASE("BufferChef() require_char 1") {
    auto chef = TestBufferChef();
    chef.test_reset("0123456789", true);
    CHECK(Status::Done == chef.read_char('0'));
    CHECK(chef.test_string() == "123456789");
    CHECK(Status::Failed == chef.read_char('0'));
    CHECK(chef.test_string() == "123456789");
    chef.advance(9);
    CHECK(Status::EndOfData == chef.read_char('0'));
    CHECK(chef.test_string() == "⟨NONE⟩");
}

TEST_CASE("BufferChef() expect/advance") {
    auto chef = TestBufferChef2(10, true);
    CHECK(chef.capacity() == 10);
    CHECK(chef.size() == 0);
    CHECK(chef.test_string() == "⟨NONE⟩");
    chef.expect(1);
    CHECK(chef.test_string() == "0123456789");
    chef.advance(5);
    CHECK(chef.test_string() == "56789");
    chef.expect(20);
    CHECK(chef.test_string() == "56789abcde");
    chef.advance(10);
    CHECK(chef.test_string() == "⟨NONE⟩");
    chef.expect(20);
    CHECK(chef.test_string() == "fghijklmno");
}

TEST_CASE("BufferChef() read_char") {
    auto chef = TestBufferChef2();
    chef.test_reset(10, true);
    CHECK(chef.capacity() == 10);
    CHECK(chef.test_string() == "⟨NONE⟩");
    CHECK(Status::Done == chef.read_char('0'));
    CHECK(chef.test_string() == "123456789");
    CHECK(Status::Failed == chef.read_char('0'));
    CHECK(chef.test_string() == "123456789");
    chef.advance(1);
    CHECK(Status::Failed == chef.read_char('0'));
    CHECK(chef.test_string() == "23456789");
    chef.advance(3);
    CHECK(Status::Failed == chef.read_char('0'));
    CHECK(chef.test_string() == "56789");
    chef.advance(4);
    CHECK(Status::Failed == chef.read_char('0'));
    CHECK(chef.test_string() == "9");
    chef.advance(1);
    CHECK(chef.test_string() == "⟨NONE⟩");
    CHECK(Status::Failed == chef.read_char('0'));
    CHECK(chef.test_string() == "abcdefghij");
}

TEST_CASE("BufferChef() read_string") {
    auto chef = TestBufferChef2();
    chef.test_reset(10, true);
    CHECK(chef.capacity() == 10);
    CHECK(chef.test_string() == "⟨NONE⟩");
    CHECK(Status::Done == chef.read_string(""));
    
    CHECK(chef.test_string() == "⟨NONE⟩");
    CHECK(Status::Done == chef.read_string("0"));
    CHECK(chef.test_string() == "123456789");
    CHECK(Status::Done == chef.read_string("12"));
    CHECK(chef.test_string() == "3456789");
    CHECK(Status::Done == chef.read_string("345"));
    CHECK(chef.test_string() == "6789");
    CHECK(Status::Done == chef.read_string("6789"));
    CHECK(chef.test_string() == "⟨NONE⟩");
    CHECK(Status::Done == chef.read_string(""));
    CHECK(chef.test_string() == "⟨NONE⟩");
    CHECK(Status::Failed == chef.read_string("0"));
    CHECK(chef.test_string() == "abcdefghij");
    CHECK(Status::Done == chef.read_string("abcdefghijk"));
    CHECK(chef.test_string() == "lmnopqrst");
    CHECK(Status::Done == chef.read_string("lmnopqrst"));
    CHECK(chef.test_string() == "⟨NONE⟩");
    CHECK(Status::EndOfData == chef.read_string(
        "uvwxyzABCD"
        "EFGHIJKLMN"
        "OPQRSTUVWX"
        "YZ" "?"
    ));
    CHECK(chef.test_string() == "⟨NONE⟩");
    CHECK(Status::Done == chef.read_string("uvwxyzABCD"));
    CHECK(Status::Failed == chef.read_string(
        "EFGHIJKLMN"
        "OPQRSTUVWX"
        "Y" "?"
    ));
    CHECK(chef.test_string() == "⟨NONE⟩");
    CHECK(Status::Done == chef.read_string("EFGHI"));
    CHECK(chef.test_string() == "JKLMN");
    CHECK(Status::EndOfData == chef.read_string(
             "JKLMN"
        "OPQRSTUVWX"
        "YZ" "?"
    ));
    CHECK(chef.test_string() == "⟨NONE⟩");
    CHECK(Status::Failed == chef.read_string(
             "JKLMN"
        "OPQRSTUVWX"
        "Y" "?"
    ));
    CHECK(chef.test_string() == "⟨NONE⟩");
    CHECK(Status::Done == chef.read_string("JKLMN"));
    CHECK(chef.test_string() == "OPQRS");
}

TEST_CASE("BufferChef() require_head") {
    auto chef = TestBufferChef2(10, true,
        "HEAD:?\n012" "345\n789" "abc\nHEA" "D:?\nEND"
    );
    CHECK(Status::Done == chef.require_head("HEAD", true));
    CHECK(chef.test_string() == "012");
    CHECK(Status::Done == chef.require_head("HEAD", false));
    CHECK(chef.test_string() == "?\nEND");
    CHECK(Status::Done == chef.require_endl());
    CHECK(chef.test_string() == "END");
    CHECK(Status::EndOfData == chef.require_endl());
    CHECK(chef.test_string() == "⟨NONE⟩");
}

TEST_CASE("BufferChef() require_head 2") {
    auto chef = TestBufferChef2(10, true,
        "0123456789"
        "abcdefghij"
        "klmnopq\nHE"
        "ADwxyzABCD"
    );
    CHECK(Status::EndOfData == chef.require_head("HEAD"));
    CHECK(chef.test_string() == "⟨NONE⟩");
    CHECK(Status::Done == chef.read_string("01234"));
    CHECK(chef.test_string() == "56789");
}


}

TEST_SUITE_END();

