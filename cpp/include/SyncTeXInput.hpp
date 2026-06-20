/**
 * SyncTeXInput.hpp
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

#ifndef SYNCTEX_INPUT_HPP
#define SYNCTEX_INPUT_HPP

#include <memory>
#include <string>
#include <map>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <numeric>

namespace fs = std::filesystem;

#include "SyncTeXNode.hpp"

namespace SyncTeXpp {

class Input;
using Input_p = std::shared_ptr<Input>;

class InputI9on;
using InputI9on_p = std::shared_ptr<InputI9on>;

class Parser;

class Node;
using Node_p = std::shared_ptr<Node>;
using Node_w = std::weak_ptr<Node>;

class Input
{
    InputI9on_p _i9on_p;
    
    protected:
    Input(
        int t = 0,
        std::string filename = "",
        fs::path output_directory = fs::path()
    );
    
    public:
    static Input_p make_p(
        int t = 0,
        std::string filename = "",
        fs::path output_directory = fs::path()
    );

    public:
    int t() const;
    std::string filename() const;
    fs::path output_directory() const;
 
    int max_l() const;

    /**
     * @brief The real file path
     * 
     * Lazy getter.
     * 
     * @return a std::string
     */
    std::string realname();

    bool is_valid();

    /**
     * @brief Records the given node for forward synchronization.
     * 
     * The tag of the receiver is also the tag of the node.
     * 
     * @param p 
     */
    void push_back(Node_p p);

    public:

    /**
     * @brief Applies <f> to each line of the receiver
     * 
     * @param f 
     */
    void iterate(std::function<void(int)> f);

    /**
     * @brief Applies <f> to each line of the receiver
     * 
     * @param f allows a break
     */
    void iterate(std::function<void(int, bool &)> f);

    /**
     * @brief Applies <f> to each node for the given line <l>
     * 
     * @param l 
     * @param f 
     */
    void iterate(int l, std::function<void(const Node_p &)> f);

    /**
     * @brief Applies <f> to each line and node
     * 
     * @param f 
     */
    void iterate(std::function<void(const Node_p &)> f);

    friend class Parser;

};

};

#endif // SYNCTEX_INPUT_HPP

