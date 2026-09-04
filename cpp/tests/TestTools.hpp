/**
 * TestTools.hpp
 * 
 * This file is part of the SyncTeX library.
 * It tests the class for representing nodes in a SyncTeX tree.
 * 
 * Copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#ifndef SYNCTEX_TEST_TOOLS_HPP
#define SYNCTEX_TEST_TOOLS_HPP

#undef SYNCTEX_TEST
#define SYNCTEX_TEST

#include <iostream>
#include <string>
#include <vector>

#include <filesystem>
#include <fstream>
#include <functional>
#include <stdexcept>

namespace fs = std::filesystem;

#undef SYNCTEX_TEST_VISITOR
#define SYNCTEX_TEST_VISITOR friend class TestVisitor;

#include "SyncTeXUtils.hpp"

namespace SyncTeXpp {

    inline std::ostream& operator<<(std::ostream& os, Status s) {
    switch (s) {
        case Status::Done:                  return os << "Done";                //  0: Done
        case Status::EndOfCapacity:         return os << "EndOfCapacity";       //  1: When the maximum capacity of the buffer has been reached:
        case Status::EndOfData:             return os << "EndOfData";           //  2: When the end of the synctex file has been reached:
        case Status::Failed:                return os << "Failed";              //  3: When the function could not return the value it was asked for:
        case Status::Error:                 return os << "Error";               //  4: Generic error
        case Status::ErrorFS:               return os << "ErrorFS";             //  5: Generic FS error
        case Status::ErrorSeek:             return os << "ErrorSeek";           //  6: Seek file failure
        case Status::ErrorMemory:           return os << "ErrorMemory";         //  7: Seek file failure
        case Status::ErrorDataUnexpected:   return os << "ErrorDataUnexpected"; //  8: The synctex file is corrupted
        case Status::ErrorDataMissing:      return os << "ErrorDataMissing";    //  9: The synctex file is corrupted
        case Status::ErrorDataFloat:        return os << "ErrorDataFloat";      // 10: The synctex file is corrupted
        case Status::ErrorDataUnit:         return os << "ErrorDataUnit";       // 11: The synctex file is corrupted
        case Status::ErrorDataPreamble:     return os << "ErrorDataPreamble";   // 12: Problem reading the Preamble records
        case Status::ErrorDataContent:      return os << "ErrorDataContent";    // 13: Problem reading the Content records
        case Status::ErrorDataPostamble:    return os << "ErrorDataPostamble";  // 14: Problem reading the Postamble
        case Status::ErrorDataNoAnchor:     return os << "ErrorDataNoAnchor";   // 15: Missing anchor
        case Status::ErrorDataBadRefTag:    return os << "ErrorDataBadRefTag";  // 16: Bad ref tag
        case Status::ErrorBadArgument:      return os << "ErrorBadArgument";    // 17: Parameter error
        case Status::ErrorLogical:          return os << "ErrorLogical";        // 18: Logical error, never reached in theory
    }
    return os << "Unknown"; // fallback for invalid/out-of-range values
}

namespace Test {

#include <iostream>
#include <string>
#include <vector>

class Coverage {
public:
    static std::string trim(const std::string& s) {
        std::size_t start = s.find_first_not_of(' ');
        if (start == std::string::npos) {
            return "";
        }
        std::size_t end = s.find_last_not_of(' ');
        std::string result = s.substr(start, end - start + 1);

        if (!result.empty() && result.front() == '"') {
            result.erase(result.begin());
        }
        if (!result.empty() && result.back() == '"') {
            result.pop_back();
        }

        return result;
    }

    static void enable() {
        stack().back().enabled = true;
    }

    static void disable() {
        stack().back().enabled = false;
    }

    static bool isEnabled() {
        return stack().back().enabled;
    }

    // Always returns true. Only stores `item` if recording is enabled
    // and the trimmed string is non-empty.
    static bool record(const std::string& item) {
        std::cout << "******************* COVERAGE RECORD?" << std::endl;
        if (isEnabled()) {
            std::cout << "******************* COVERAGE RECORD: " << item << std::endl;
            std::string trimmed = trim(item);
            if (!trimmed.empty()) {
                stack().back().strings.push_back(trimmed);
            }
        }
        return true;
    }

    // Always returns true. Only stores non-empty (after trim) items,
    // and only if recording is enabled.
    static bool record(const std::vector<std::string>& items) {
        std::cout << "******************* COVERAGE RECORD?" << std::endl;
        if (isEnabled()) {
            std::cout << "******************* COVERAGE RECORD:" << std::endl;
            for (const auto& item : items) {
                std::cout << item << ",";
            }
            std::cout << std::endl;
            auto& current = stack().back().strings;
            current.reserve(current.size() + items.size());
            for (const auto& item : items) {
                std::string trimmed = trim(item);
                if (!trimmed.empty()) {
                    current.push_back(trimmed);
                }
            }
        }
        return true;
    }

    static void push() {
        std::cout << "$$$$$$$$$$$$$$$ PUSH" << std::endl;
        bool inheritedEnabled = isEnabled();
        stack().push_back(Level{ {}, inheritedEnabled });
    }

    static void pop() {
        if (stack().size() > 1) {
            stack().pop_back();
        }
    }

    static std::vector<std::string> entries() {
        std::vector<std::string> result;
        for (const auto& level : stack()) {
            result.insert(result.end(), level.strings.begin(), level.strings.end());
        }
        return result;
    }

    static bool find(const std::vector<std::string>& sequence) {
        if (sequence.empty()) {
            return true;
        }
        auto all = entries();
        std::size_t seqIdx = 0;
        auto what = trim(sequence[seqIdx]);
        for (const auto& item: all) {
            if (item == what) {
                ++seqIdx;
                if (seqIdx == sequence.size()) {
                    // all the sequence element were found
                    return true;
                }
                what = trim(sequence[seqIdx]);
            }
        }
        return false;
    }

    static bool print(const std::string& message = "", std::ostream& os = std::cout) {
        os << "COVERAGE: " << message << std::endl;

        const auto& s = stack();
        for (std::size_t i = 0; i < s.size(); ++i) {
            os << "    Level " << i
               << " (" << (s[i].enabled ? "enabled" : "disabled") << "): [";
            for (std::size_t j = 0; j < s[i].strings.size(); ++j) {
                os << s[i].strings[j];
                if (j + 1 < s[i].strings.size()) {
                    os << ", ";
                }
            }
            os << "]" << std::endl;
        }
        return true;
    }

private:
    struct Level {
        std::vector<std::string> strings;
        bool enabled;
    };

    static std::vector<Level>& stack() {
        static std::vector<Level> instance{ Level{ {}, true } };
        return instance;
    }
};

};

};

// Macro pour stringifier les arguments
#define SYNCTEX_STRINGIFY(WHAT) #WHAT

// Macro pour stringifier les arguments après extensions
#define SYNCTEX_X_STRINGIFY(WHAT) SYNCTEX_STRINGIFY(WHAT)

// Macro pour stringifier les arguments
#define SYNCTEX_STDSTRINGIFY(WHAT) std::string(#WHAT)

// Macro pour stringifier les arguments après extensions
#define SYNCTEX_X_STDSTRINGIFY(WHAT) SYNCTEX_STDSTRINGIFY(WHAT)

// Count how many arguments were passed (supports up to 10 here; extend as needed)
#define SYNCTEX_GET_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,NAME,...) NAME

#define SYNCTEX_FOR_EACH_0(HOW, WHAT)       HOW(WHAT)
#define SYNCTEX_FOR_EACH_1(HOW, WHAT, ...)  HOW(WHAT), SYNCTEX_FOR_EACH_0(HOW, __VA_ARGS__)
#define SYNCTEX_FOR_EACH_2(HOW, WHAT, ...)  HOW(WHAT), SYNCTEX_FOR_EACH_1(HOW, __VA_ARGS__)
#define SYNCTEX_FOR_EACH_3(HOW, WHAT, ...)  HOW(WHAT), SYNCTEX_FOR_EACH_2(HOW, __VA_ARGS__)
#define SYNCTEX_FOR_EACH_4(HOW, WHAT, ...)  HOW(WHAT), SYNCTEX_FOR_EACH_3(HOW, __VA_ARGS__)
#define SYNCTEX_FOR_EACH_5(HOW, WHAT, ...)  HOW(WHAT), SYNCTEX_FOR_EACH_4(HOW, __VA_ARGS__)
#define SYNCTEX_FOR_EACH_6(HOW, WHAT, ...)  HOW(WHAT), SYNCTEX_FOR_EACH_5(HOW, __VA_ARGS__)
#define SYNCTEX_FOR_EACH_7(HOW, WHAT, ...)  HOW(WHAT), SYNCTEX_FOR_EACH_6(HOW, __VA_ARGS__)
#define SYNCTEX_FOR_EACH_8(HOW, WHAT, ...)  HOW(WHAT), SYNCTEX_FOR_EACH_7(HOW, __VA_ARGS__)
#define SYNCTEX_FOR_EACH_9(HOW, WHAT, ...)  HOW(WHAT), SYNCTEX_FOR_EACH_8(HOW, __VA_ARGS__)

#define SYNCTEX_STRINGIFY_EACH(...) \
    SYNCTEX_GET_MACRO(__VA_ARGS__, \
        SYNCTEX_FOR_EACH_9, \
        SYNCTEX_FOR_EACH_8, \
        SYNCTEX_FOR_EACH_7, \
        SYNCTEX_FOR_EACH_6, \
        SYNCTEX_FOR_EACH_5, \
        SYNCTEX_FOR_EACH_4, \
        SYNCTEX_FOR_EACH_3, \
        SYNCTEX_FOR_EACH_2, \
        SYNCTEX_FOR_EACH_1, \
        SYNCTEX_FOR_EACH_0 \
    ) (SYNCTEX_STRINGIFY, __VA_ARGS__)

#define SYNCTEX_X_STRINGIFY_EACH(...) \
    SYNCTEX_GET_MACRO(__VA_ARGS__, \
        SYNCTEX_FOR_EACH_9, \
        SYNCTEX_FOR_EACH_8, \
        SYNCTEX_FOR_EACH_7, \
        SYNCTEX_FOR_EACH_6, \
        SYNCTEX_FOR_EACH_5, \
        SYNCTEX_FOR_EACH_4, \
        SYNCTEX_FOR_EACH_3, \
        SYNCTEX_FOR_EACH_2, \
        SYNCTEX_FOR_EACH_1, \
        SYNCTEX_FOR_EACH_0 \
    ) (SYNCTEX_X_STRINGIFY, __VA_ARGS__)

#define SYNCTEX_STDSTRINGIFY_EACH(...) \
    SYNCTEX_GET_MACRO(__VA_ARGS__, \
        SYNCTEX_FOR_EACH_9, \
        SYNCTEX_FOR_EACH_8, \
        SYNCTEX_FOR_EACH_7, \
        SYNCTEX_FOR_EACH_6, \
        SYNCTEX_FOR_EACH_5, \
        SYNCTEX_FOR_EACH_4, \
        SYNCTEX_FOR_EACH_3, \
        SYNCTEX_FOR_EACH_2, \
        SYNCTEX_FOR_EACH_1, \
        SYNCTEX_FOR_EACH_0 \
    ) (SYNCTEX_STDSTRINGIFY, __VA_ARGS__)

#define SYNCTEX_X_STDSTRINGIFY_EACH(...) \
    SYNCTEX_GET_MACRO(__VA_ARGS__, \
        SYNCTEX_FOR_EACH_9, \
        SYNCTEX_FOR_EACH_8, \
        SYNCTEX_FOR_EACH_7, \
        SYNCTEX_FOR_EACH_6, \
        SYNCTEX_FOR_EACH_5, \
        SYNCTEX_FOR_EACH_4, \
        SYNCTEX_FOR_EACH_3, \
        SYNCTEX_FOR_EACH_2, \
        SYNCTEX_FOR_EACH_1, \
        SYNCTEX_FOR_EACH_0 \
    ) (SYNCTEX_X_STDSTRINGIFY, __VA_ARGS__)

#undef SYNCTEX_R_COVERAGE
#undef SYNCTEX_COVERAGE
#undef SYNCTEX_COVERAGE_PRINT
#define SYNCTEX_R_COVERAGE(...)     SyncTeXpp::Test::Coverage::record({__VA_ARGS__})
#define SYNCTEX_COVERAGE(...)       SyncTeXpp::Test::Coverage::record({SYNCTEX_X_STDSTRINGIFY_EACH(__VA_ARGS__)})
#define SYNCTEX_COVERAGE_PUSH(...)  SyncTeXpp::Test::Coverage::push(); SyncTeXpp::Test::Coverage::record({SYNCTEX_X_STDSTRINGIFY_EACH(__VA_ARGS__)})
#define SYNCTEX_COVERAGE_POP()      SyncTeXpp::Test::Coverage::pop()
#define SYNCTEX_COVERAGE_PRINT(MSG) SyncTeXpp::Test::Coverage::print(MSG)

#define SYNCTEX_COVERAGE_CHECK(...)       CHECK( SyncTeXpp::Test::Coverage::find({SYNCTEX_X_STDSTRINGIFY_EACH(__VA_ARGS__)}))
#define SYNCTEX_COVERAGE_CHECK_NOT(...)   CHECK(!SyncTeXpp::Test::Coverage::find({SYNCTEX_X_STDSTRINGIFY_EACH(__VA_ARGS__)}))
#define SYNCTEX_COVERAGE_REQUIRE(...)     REQUIRE( SyncTeXpp::Test::Coverage::find({SYNCTEX_X_STDSTRINGIFY_EACH(__VA_ARGS__)}))
#define SYNCTEX_COVERAGE_REQUIRE_NOT(...) REQUIRE(!SyncTeXpp::Test::Coverage::find({SYNCTEX_X_STDSTRINGIFY_EACH(__VA_ARGS__)}))

namespace SyncTeXpp
{
class Tester {
public:
    // Creates a temporary file named `fileName` containing `fileContent`,
    // then calls `func` with the file's full path. The file is deleted
    // afterward, even if `func` throws.
    static void test_synctex(
        const std::string& fileName,
        const std::string& fileContent,
        const std::function<void(const std::string&)>& func
    ) {
        fs::path fullPath = fs::temp_directory_path() / fileName;
        {
            fs::path p = fullPath;
            p.replace_extension(".synctex");
            std::ofstream out(p, std::ios::out | std::ios::trunc);
            if (!out) {
                throw std::runtime_error("Test::test_synctex: could not create file: " + p.string());
            }
            out << fileContent;
        } // file is flushed and closed here

        try {
            func(fullPath.string());
        } catch (...) {
            fs::remove(fullPath);
            throw; // preserve the original exception after cleanup
        }

        fs::remove(fullPath);
    }
};

};

#endif // SYNCTEX_TEST_TOOLS_HPP
