/**
 * SyncTeXInput.cpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

// #pragma once

/**
 * @brief Nodes to represent SyncTeX "Input:"" data.
 * 
 * Used by a Scanner instance.
 */

#include "SyncTeXInput.hpp"

namespace SyncTeXpp {

class InputI9on {
    friend class Input;
    int _t;
    std::string _filename;
    fs::path    _relative;
    fs::path    _directory;
    fs::path    _output_directory;
    std::string _realname;
    std::map<int, std::vector<Node_w>> _nodes_by_line;
    /**
     * @brief 
     * 
     *
     * @param t the tag
     * @param filename the filename recorded in the .synctex file
     * @param output_directory the parent directory of the pdf output.
     */
    public:
    InputI9on(
        int t,
        std::string filename,
        fs::path output_directory
    ):
        _t(t),
        _filename(filename),
        _output_directory(output_directory)
    {
        std::error_code ec;
        fs::path p;
        auto fp = fs::path(_filename);
        auto it = std::find(fp.begin(), fp.end(), ".");
        if (it != fp.end() && std::find(std::next(it), fp.end(), ".") == fp.end()) {
            // The path contains exactly one "." component
            // collect what comes before and after the "."
            // in _directory and _relative
            auto it2 = fp.begin();
            do {
                _directory /= *it2;
                std::advance(it2, 1);
            } while (it2 != it);
            std::advance(it, 1);
            do {
                _relative /= *it;
                std::advance(it, 1);
            } while (it == fp.end());
        } else {
            // _relative is just the last component
            // directory is the parent directory (the other components)
            _relative = fp.filename();
            _directory = fp.parent_path();
        }
        if (_relative.empty()) return; // very unlikely
        p = fs::canonical(_output_directory / _relative, ec);
        if (!ec && fs::is_regular_file(p)) {
            _realname = p;
            return;
        }
        if (!_output_directory.empty()) {
            p = fs::canonical(_relative, ec);
            if (!ec && fs::is_regular_file(p)) {
                _realname = p;
                return;
            }
        }
        if (!fp.is_absolute()) {
            p = fs::canonical(fp, ec);
            if (!ec && fs::is_regular_file(p)) {
                _realname = p;
                return;
            }
            if (!_output_directory.empty()) {
                p = fs::canonical(_output_directory / fp, ec);
                if (!ec && fs::is_regular_file(p)) {
                    _realname = p;
                    return;
                }
            }
        }
        p = fs::canonical(fp, ec);
        if (!ec && fs::is_regular_file(p)) {
            _realname = p;
            return;
        }
    }
};

Input::Input(
    int t,
    std::string filename,
    fs::path output_directory
) {
    _i9on_p = std::make_shared<InputI9on>(t, filename, output_directory);
}

Input_p Input::make_p(
    int t,
    std::string filename,
    fs::path output_directory
) {
    return std::shared_ptr<Input>( new Input(
        t, filename, output_directory
    ) );
};

bool Input::is_valid() {
    return !_i9on_p->_realname.empty();
}

int Input::t() const {
    return _i9on_p->_t;
}

std::string Input::filename() const {
    return _i9on_p->_filename;
}

fs::path Input::output_directory() const {
    return _i9on_p->_output_directory;
}

int Input::max_l() const {
    return _i9on_p->_nodes_by_line.empty() ? 0 : _i9on_p->_nodes_by_line.rbegin()->first;
}

std::string Input::realname() {
    return _i9on_p->_realname;
}

void Input::push_back(Node_p p) {
    if (p && p->t() == t()) {
        auto line = p->l();
        auto where = _i9on_p->_nodes_by_line.find(line);
        if (where != _i9on_p->_nodes_by_line.end()) {
            where->second.push_back(p);
        } else {
            _i9on_p->_nodes_by_line[line] = { p };
        }
    };
}

void Input::iterate(std::function<void(int)> f) {
    for (const auto& pair : _i9on_p->_nodes_by_line) {
        f(pair.first);
    }
}

void Input::iterate(std::function<void(int, bool &)> f) {
    bool stop = false;
    for (const auto& pair : _i9on_p->_nodes_by_line) {
        f(pair.first, stop);
        if (stop) {
            break;
        }
    }
}

void Input::iterate(int l, std::function<void(const Node_p &)> f) {
    auto where = _i9on_p->_nodes_by_line.find(l);
    if (where != _i9on_p->_nodes_by_line.end()) {
        for (const auto &p: where->second) {
            auto pp = p.lock();
            if (pp) f(pp);
        }
    }
}

void Input::iterate(std::function<void(const Node_p &)> f) {
    for (const auto& pair : _i9on_p->_nodes_by_line) {
        for (const auto &p: pair.second) {
            auto pp = p.lock();
            if (pp) f(pp);
        }
    }
}

};
