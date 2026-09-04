/**
 * SyncTeXManager.hpp
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

#ifndef SYNCTEX_MANAGER_HPP
#define SYNCTEX_MANAGER_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <functional>
#include <filesystem>

namespace fs = std::filesystem;

#include "SyncTeXPrefix.hpp"

namespace SyncTeXpp {

class Input;
using Input_p = std::shared_ptr<Input>;

class Sheet;
using Sheet_p = std::shared_ptr<Sheet>;

class Form;
using Form_p = std::shared_ptr<Form>;

class Reader;
using Reader_p = std::shared_ptr<Reader>;

class Below;
using Below_p = std::shared_ptr<Below>;

struct Flc {
    fs::path file_path;
    int l;
    int c;
};

template <typename T>
using maker_tlc_hv_w_t = T(*)(
    int t, int l, int c,
    int h, int v,
    int w
);

template <typename T>
using maker_tlc_hv_t = T(*)(
    int t, int l, int c,
    int h, int v
);

template <typename T>
using maker_tlc_hv_WHD_t = T(*)(
    int t, int l, int c,
    int h, int v,
    int w, int g, int d
);

class Parser;

class Manager;
using Manager_p = std::shared_ptr<Manager>;

class Manager:
    public std::enable_shared_from_this<Manager>
{
    public:
        std::vector<Node_p> edit(
        int page,
        float h,
        float v
    );
    std::vector<Node_p> view(
        std::string_view name,
        int t,
        int l,
        int c
    );
    std::vector<Node_p> view(
        std::string_view name,
        int t,
        int l,
        int c,
        std::function<bool(Node_p, Node_p)>
    );

    private:
    class I9on; // I9on -> Implementation
    using I9on_p = std::shared_ptr<I9on>;
    I9on_p _i9on_p;

    protected:
    Manager(
        fs::path output,
        fs::path build_directory,
        bool parse = true
    );
    Manager(
        fs::path output,
        bool parse = true
    );

    public:
    static Manager_p make_p(
        fs::path output,
        fs::path build_directory,
        bool parse = true
    );

    static Manager_p make_p(
        fs::path output,
        bool parse = true
    );

    int version() const;
    int pre_magnification() const;
    int pre_unit() const;
    int pre_x_offset() const;
    int pre_y_offset() const;
    float unit() const;
    int x_offset() const;
    int y_offset() const;

    const fs::path output_directory() const;

    Status parse(int &error_count);

    /** Auxiliary reader delegate, same lifetime as the owner */
    Reader_p reader_p() const;

    Node_p sheet_p(int page) const;
    Node_p form_p(int page) const;
    Input_p input_p(int page) const;

    friend class Parser;

    SYNCTEX_TEST_VISITOR
};

};

#endif // SYNCTEX_MANAGER_HPP

#if 0

/**
 *  The synctex scanner is the root object.
 *
 *  Is is initialized with the contents of a text file or a gzipped file.
 *  The buffer_.* are first used to parse the text.
 */

#ifndef SYNCTEX_NUMBER_OF_MATE_LISTS
#define SYNCTEX_NUMBER_OF_MATE_LISTS (1024)
#endif
class Manager:
    public std::enable_shared_from_this<Manager>
{

/**
 * @brief Manager display switcher getter
 *
 * @param scanR
 * @return int
 * @see `synctex_scanner_set_display_switcher`
 */
    int display_switcher() const { return _display_switcher; };
/**
 * @brief Manager display switcher setter.
 *
 * @param scanR
 * @param switcher
 * If the switcher is 0, `synctex_node_display` is disabled.
 * If the switcher is <0, `synctex_node_display` has no limit.
 * If the switcher is >0, only the first switcher (as number) nodes are displayed.
 */
    void set_display_switcher(int switcher) { _display_switcher = switcher; };
    private:
    void push_back(Input_p &input_p, Node_p node_p);
    void push_back_hbox(Node_p sheet_p, Node_p node_p);
    void foreach_hbox(int page, std::function<void(Node_p)>);
    void foreach_hbox(int page, std::function<void(Node_p, bool&)>);
    
/*  Here are gathered all the possible status that the next scanning functions will return.
 *  All these functions return a status, and pass their result through pointers.
 *  Negative values correspond to errors.
 *  The management of the buffer is causing some significant overhead.
 *  Every function that may access the buffer returns a status related to the buffer and file state.
 *  status >= Status::Done means the function worked as expected
 *  Status::Done < status means the function did not work as expected
 *  Status::Failed == status means the function did not work as expected but there is still some material to parse.
 *  Status::EndOfData == status means the function did not work as expected and there is no more material.
 *  Status::EndOfData<status means an error
 */
    Status get_available(size_t& actual, size_t expected) {
        if (_reader_p) {
            return _reader_p->get_available(actual, expected);
        }
        return Status::Error;
    }

    // template <typename T>
    // Status Manager::make_tlc_hv_WHD(Node_p &node_p);
// template <typename T>
// Status Manager::make_tlc_hv_WHD(Node_p &node_p)
// {
//     Status status;
//     int t, line, column;
//     if (Status::Done < (status = decode(t, l, c))) {
//         return status;
//     }
//     int h, v;
//     if (Status::Done < (status = decode(h, v))) {
//         return status;
//     }
//     int w, height, depth;
//     if (Status::Done < (status = decode(width, height, depth))) {
//         return status;
//     }
//     node_p = T::make_p(
        t, l, c,
        h, v,
        width, height, depth
    );
//     if (node_p) {
//         if (Status::Done < next_l()) {
//             // std::cerr << "Missing end of vbox." << std::endl;
//             return Status::Error;
//         }
//         node_p->setup_visible();
//         return Status::Done;
//     }
//     next_l();
//     return Status::Error;
// }

    template <typename T>
    Status make_tlc_hv(Node_p &, maker_tlc_hv_t<T>);
    template <typename T>
    Status make_tlc_hv_WHD(Node_p &, maker_tlc_hv_WHD_t<T>);
    Status make_vbox(Node_p &node_p);
    Status make_hbox(Node_p &node_p);
    Status make_void_vbox(Node_p &node_p);
    Status make_void_hbox(Node_p &node_p);
    Status make_kern(Node_p &node_p);
    Status make_glue(Node_p &node_p);
    Status make_rule(Node_p &node_p);
    Status make_math(Node_p &node_p);
    Status make_boundary(Node_p &node_p);
    Status make_extra_box_bdry(Node_p &node_p);
    Status make_ref(Node_p &node_p);

    Status require_endl();

    Status read_string(const std::string& keyword) {
        if (_reader_p) {
            return _reader_p->read_string(keyword);
        }
        return Status::Error;
    }

    Status scan_float_and_dimension(float &);

    Status parse_sfi();

    /**
     *  The content of the sheet with given page number.
     *  - parameter scanner: a scanner.
    *  - parameter tag: an integer identifier.
    *  - returns: a box node.
    *  - author: JL
    */
    Node_p below_of_form(int t);

    private:
    Status replace_ref(Node_p);
    public:
    std::vector<Input_p> get_inputs(std::string_view);
    std::vector<int> get_tags(std::string_view);
    fs::path get_filepath(int t);
    std::string get_filename(int t);

    std::vector<Node_p> mates_of(const Node_p p) const;

    static Node_p smallest_container_v2(Node_p node1_p, Node_p node2_p);

};


};


#endif

