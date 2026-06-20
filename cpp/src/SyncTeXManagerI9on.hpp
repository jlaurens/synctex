/**
 * SyncTeXManagerI9on.hpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

/**
 * @brief Manager of the SyncTeX data.
 *
 */

#ifndef SYNCTEX_MANAGER_I9ON_HPP
#define SYNCTEX_MANAGER_I9ON_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <functional>
#include <filesystem>

namespace fs = std::filesystem;

#include "SyncTeXManager.hpp"
#include "SyncTeXReader.hpp"
#include "SyncTeXTree.hpp"
#include "SyncTeXInput.hpp"

namespace SyncTeXpp {

class Manager::I9on
{
    Reader_p _reader_p;
    /** various flags */
    struct {
        /**  Whether the scanner has parsed its underlying synctex file. */
        unsigned has_parsed : 1;
        /*  Whether the scanner has parsed the postamble. */
        unsigned postamble : 1;
        /*  alignment */
        unsigned reserved : sizeof(unsigned) - 2;
    } _flags;
    /**
     * @brief Version
     * 
     */
    int _version;
    /** magnification from the synctex preamble */
    int _pre_magnification = 1000;
    /** unit from the synctex preamble */
    int _pre_unit = 8192;
    /** X offset from the synctex preamble */
    int _pre_x_offset = 578;
    /** Y offset from the synctex preamble */
    int _pre_y_offset = 578;
    /** Number of records, from the synctex postamble */
    int _count = 0;
    /** real unit, from synctex preamble or post scriptum */
    float _unit;
    /** X offset, from synctex preamble or post scriptum */
    float _x_offset = 6.027e23f;
    /** Y Offset, from synctex preamble or post scriptum */
    float _y_offset = 6.027e23f;
    std::unordered_map<int,Input_p> _inputs_by_tag;
    std::unordered_map<int,Sheet_p> _sheet_p_by_tag;
    std::unordered_map<int,Form_p>  _form_p_by_tag;
    std::unordered_map<int,std::vector<Node_p>>  _hboxes_by_page;
    /** The first form ref node in sheet, its friends are the other form ref nodes in sheets */
    Node_p _ref_in_sheet_p;
    /** The first form ref node in form, its friends are the other form ref nodes in form */
    Node_p _ref_in_form_p;
    /** The display switcher value*/
    int _display_switcher = 1000;
    /** The display prompt */
    std::string _display_prompt;

    Input_p     _input_p;
    int         _input_tag = 0;

    I9on(
        fs::path output,
        fs::path build_directory
    ) {
        _reader_p = Reader::make_p(output, build_directory);
    }
    
    Input_p input_p(int page) const {
        auto found = _inputs_by_tag.find(page);
        if (found != _inputs_by_tag.end()) {
            return found->second;
        }
        return nullptr;
    }
    void input_push_back(Below_p b_p);
    bool read_input(int &error_count);
    friend class Manager;
    friend class Parser;
    SYNCTEX_TEST_VISITOR
};

inline void Manager::I9on::input_push_back(Below_p b_p) {
    if (b_p) {
        if (_input_p && _input_tag == b_p->t()) {
            _input_p->push_back(b_p);
            return;
        }
        auto p = input_p(b_p->t());
        if (p) {
            _input_p = p;
            _input_tag = p->t();
            p->push_back(b_p);
        }
    }
}
inline bool Manager::I9on::read_input(int &error_count)
{
    if (Status::Done == _reader_p->read_string(Prefix::INPUT)) {
        int t = 0;
        std::string s;
        if ((Status::Done == _reader_p->decode(t)) && (Status::Done == _reader_p->decode(s, ':'))) {
            auto i_p = Input::make_p(t, s, _reader_p->output_directory());
            _inputs_by_tag[t] = i_p;
            _input_tag = t;
            _input_p = i_p;
            return read_input(error_count);
        } else {
            _reader_p->require_endl();
            ++error_count;
        }
    }
    return false;
}

};

#endif // SYNCTEX_MANAGER_I9ON_HPP
