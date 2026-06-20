/**
 * SyncTeXPrefix.hpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

/**
 * @brief Scanner of the SyncTeX data.
 *
 */

#ifndef SYNCTEX_PREFIX_HPP
#define SYNCTEX_PREFIX_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <functional>
#include <filesystem>

namespace fs = std::filesystem;

#include "SyncTeXUtils.hpp"

namespace SyncTeXpp {

namespace Prefix
{
    constexpr const char BEGIN_SHEET    = '{';
    constexpr const char END_SHEET      = '}';
    constexpr const char BEGIN_FORM     = '<';
    constexpr const char END_FORM       = '>';
    constexpr const char BEGIN_VBOX     = '[';
    constexpr const char END_VBOX       = ']';
    constexpr const char BEGIN_HBOX     = '(';
    constexpr const char END_HBOX       = ')';
    constexpr const char ANCHOR         = '!';
    constexpr const char VOID_VBOX      = 'v';
    constexpr const char VOID_HBOX      = 'h';
    constexpr const char KERN           = 'k';
    constexpr const char GLUE           = 'g';
    constexpr const char RULE           = 'r';
    constexpr const char MATH           = '$';
    constexpr const char FORM_REF       = 'f';
    constexpr const char BOUNDARY       = 'x';
    constexpr const char CHARACTER      = 'c';
    constexpr const char COMMENT        = '%';
    constexpr const char NONE           = '\0';
    constexpr const std::string_view VERSION        = "SyncTeX version:";
    constexpr const std::string_view INPUT          = "Input:";
    constexpr const std::string_view MAGNIFICATION  = "Magnification:";
    constexpr const std::string_view UNIT           = "Unit:";
    constexpr const std::string_view X_OFFSET       = "X Offset:";
    constexpr const std::string_view Y_OFFSET       = "Y Offset:";
    constexpr const std::string_view CONTENT        = "Content:";
};

};
#endif // SYNCTEX_PREFIX_HPP
