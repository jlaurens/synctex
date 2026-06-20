/**
 * Reader_0.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

/**
 * Config_1.cpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#ifndef SYNCTEX_TESTS_BUFFER_CHEF_HPP
#define SYNCTEX_TESTS_BUFFER_CHEF_HPP

#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <iterator>


namespace fs = std::filesystem;

#include "SyncTeXBufferChef.hpp"
 
namespace SyncTeXpp {

class TestBufferChef: public BufferChef
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
    Restore_f save() override {
        int where = _where - size() + position();
        // std::cout << "SAVED: " << std::string(_str.begin() + where, _str.end()) << std::endl;
        return [this, where](){
            _where = where;
            this->set_size(0);
            // std::cout << "RESTORED TO:" << std::string(_str.begin() + _where, _str.end()) << std::endl;
        };
    }
    public:
    TestBufferChef(
        int capacity = -1,
        const std::string_view & str = ""
    ): BufferChef(capacity),
    _str(str) {
        reset(capacity, true);
    }
        
    void test_reset(
        const std::string_view & str,
        int capacity = 0,
        bool force=false
    ) {
        _str = str;
        reset(capacity, force);
        _where = 0;
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
    Status test_decode(int & value, char separator = '\0') {
        const char * p = mutable_data();
        if (p && capacity() > 0) {
            if (Status::Done < expect(1)) return Status::Failed;
            auto c = p[position()];
            std::cout << "STEP c 1 -> " << c << std::endl;
            if (separator) {
                std::cout << "STEP separator " << separator << std::endl;
                if (c != separator) return Status::Failed;
                advance(1);
                if (Status::Done < expect(1)) return Status::Failed;
                c = p[position()];
                std::cout << "STEP c 2 " << c << std::endl;
            }
            std::cout << "STEP 3" << std::endl;
            bool negative = false;
            if (c == '-') {
                std::cout << "STEP '-'" << std::endl;
                negative = true;
                advance(1);
                if (Status::Done < expect(1)) return Status::Failed;
                c = p[position()];
                std::cout << "STEP c 4 " << c << std::endl;
            }
            std::cout << "STEP 5 " << std::endl;
            if (!std::isdigit(c)) return Status::Failed;
            value = c - '0';
            advance(1);
            while (Status::Done == expect(1)) {
                c = p[position()];
                if (std::isdigit(c)) {
                    value = 10 * value + (c - '0');
                    advance(1);
                    continue;
                }
                // c is not a digit, we can return
                if (negative) value = -value;
                std::cout << "STEP DONE" << negative << value << std::endl;
                return Status::Done;
            } // end of while ((Status::Done == expect(1)))
        } // end of if (p && ...)
        return Status::EndOfData;
    }
};

};

#endif // SYNCTEX_TESTS_BUFFER_CHEF_HPP