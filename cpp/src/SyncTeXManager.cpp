/**
 * SyncTeXManager.cpp
 * 
 * This file is part of the SyncTeX library.
 * It implements the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include <algorithm>
#include <iostream>
#include <string>
#include <zlib.h>
#include <charconv>

#include "SyncTeXUtils.hpp"
#include "SyncTeXManager.hpp"
#include "SyncTeXManagerI9on.hpp"
#include "SyncTeXReader.hpp"
#include "SyncTeXParser.hpp"

#if defined(SYNCTEX_TEST)
#   include "TestTools.hpp"
#endif

namespace SyncTeXpp {

Manager::Manager(
    fs::path output,
    fs::path build_directory,
    bool parse
) {
    _i9on_p = std::shared_ptr<Manager::I9on>(
        new Manager::I9on(output, build_directory)
    );
    if (parse) {
        int error_count = 0;
        this->parse(error_count);
    }
}

Manager::Manager(
    fs::path output,
    bool parse
) {
    _i9on_p = std::shared_ptr<Manager::I9on>(
        new Manager::I9on(output, fs::path())
    );
    // _reader_p = Reader::make_p(output, fs::path());
    if (parse) {
        int error_count = 0;
        this->parse(error_count);
    }
}

Manager_p Manager::make_p(
    fs::path output,
    fs::path build_directory,
    bool parse
) {
    return std::shared_ptr<Manager>(
        new Manager(
            output,
            build_directory,
            parse
        )
    );
}

Manager_p Manager::make_p(
    fs::path output,
    bool parse
) {
    return std::shared_ptr<Manager>(
        new Manager(
            output,
            fs::path(),
            parse
        )
    );
}


// Status Manager::I9on::parse(int &error_count) {
//     std::cout << "########## PARSE" << std::endl;
//     if (_flags.has_parsed) return Status::Done;
//     _flags.has_parsed = 1;
//     if (Status::Done != _reader_p->read_string(Prefix::VERSION)) {
//         ++error_count;
//         return Status::ErrorDataContent;
//     }
//     int i = 0;
//     if (Status::Done != _reader_p->decode(i)) {
//         ++error_count;
//         return Status::ErrorDataContent;
//     }
//     _reader_p->require_endl();
//     _version = i;
//     std::cout << "########## VERSION" << i << std::endl;
//     read_input(error_count);
//     if (error_count) {
//         return Status::ErrorDataPreamble;
//     }

//     return Status::Done;
// }

Status Manager::parse(int &error_count) {
    if (_i9on_p->_flags.has_parsed) return Status::Done;
    _i9on_p->_flags.has_parsed = 1;
    if (Status::Done != _i9on_p->_reader_p->read_string(Prefix::VERSION)) {
        ++error_count;
        return Status::ErrorDataPreamble;
    }
    int i = 0;
    if (Status::Done != _i9on_p->_reader_p->decode(i)) {
        ++error_count;
        return Status::ErrorDataPreamble;
    }
    _i9on_p->_reader_p->require_endl();
    _i9on_p->_version = i;
    _i9on_p->read_input(i);
    #define SYNCTEX_PREAMBLE_READ(WHAT, WHERE)                          \
    if (Status::Done != _i9on_p->_reader_p->read_string(Prefix::WHAT)) { \
        ++error_count;                                \
        return Status::ErrorDataPreamble;              \
    }                                                   \
    if (Status::Done != _i9on_p->_reader_p->decode(i)) { \
        ++error_count;                   \
        return Status::ErrorDataPreamble; \
    }                                      \
    _i9on_p->_reader_p->require_endl();     \
    _i9on_p->WHERE = i
    SYNCTEX_PREAMBLE_READ(MAGNIFICATION, _pre_magnification);
    SYNCTEX_PREAMBLE_READ(UNIT, _pre_unit);
    SYNCTEX_PREAMBLE_READ(X_OFFSET, _pre_x_offset);
    SYNCTEX_PREAMBLE_READ(Y_OFFSET, _pre_y_offset);
    if (Status::Done != _i9on_p->_reader_p->decode(i,'!')) {
        ++error_count;
        return Status::ErrorDataPreamble;
    }
    _i9on_p->_reader_p->require_endl();
    if (Status::Done != _i9on_p->_reader_p->read_string(Prefix::CONTENT)) {
        ++error_count;
        return Status::ErrorDataContent;
    }
    _i9on_p->_reader_p->require_endl();
    if (Status::Done != _i9on_p->_reader_p->decode(i,'!')) {
        ++error_count;
        return Status::ErrorDataContent;
    }
    _i9on_p->_reader_p->require_endl();
    while (true) {
        auto status = Status::Done;
        #define SYNCTEX_READ(WHAT)  \
            ((SYNCTEX_X_COVERAGE(WHAT)) && Status::Done == (status = _i9on_p->_reader_p->read_char(Prefix::WHAT)))
        #define SYNCTEX_READ_ALT(WHAT)  \
            (Status::Done == (status = _i9on_p->_reader_p->read_char(Prefix::WHAT)))
        if (SYNCTEX_READ(BEGIN_SHEET)) {
            if (Status::Done < (status = Parser::parse_sheet(_i9on_p, error_count))) {
                return status;
            }
        } else if (SYNCTEX_READ(BEGIN_FORM)) {
            if (Status::Done < (status = Parser::parse_form(_i9on_p, error_count))) {
                return status;
            }
        } else if (SYNCTEX_READ(ANCHOR)) {
            if (Status::Done < _i9on_p->_reader_p->require_endl()) {
                // std::cerr << "Missing anchor." << std::endl;
                return Status::ErrorDataNoAnchor;
            }
        } else if (SYNCTEX_READ(COMMENT)) {
            _i9on_p->_reader_p->require_endl();
        } else if (_i9on_p->read_input(error_count)) {
            ;
        } else if (Status::Done == (status = _i9on_p->_reader_p->read_string("Postamble:"))) {
            _i9on_p->_flags.postamble = 1;
            return status;
        } else if (Status::Done < (status = _i9on_p->_reader_p->skip_endl())) {
            // Coverage No file
            ++error_count;
            return status;
        } else if (Status::Done < _i9on_p->_reader_p->expect(1)) { //At least 1 more character
            // std::cerr << "Incomplete synctex file, postamble missing." << std::endl;
            ++error_count;
            return Status::ErrorDataPostamble;
        }
    }
    auto parser_p = Parser::make_p(_i9on_p);
    auto status = parser_p->parse_content(error_count);
    #warning Missingscan_postamble
    return status;
}

const fs::path Manager::output_directory() const {
    if(_i9on_p->_reader_p) {
        return _i9on_p->_reader_p->_output_directory;
    }
    return fs::path();
}

Reader_p Manager::reader_p() const
{
    return _i9on_p->_reader_p;
}
int Manager::version() const {
    return _i9on_p->_version;
}
int Manager::pre_magnification() const {
    return _i9on_p->_pre_magnification;
}
int Manager::pre_unit() const {
    return _i9on_p->_pre_unit;
}
int Manager::pre_x_offset() const
{
    return _i9on_p->_pre_x_offset;
}
int Manager::pre_y_offset() const
{
    return _i9on_p->_pre_y_offset;
}
int Manager::x_offset() const
{
    return _i9on_p->_x_offset;
}
int Manager::y_offset() const
{
    return _i9on_p->_y_offset;
}
// float Manager::magnification() const
// {
//     return _i9on_p->_unit;
// }
float Manager::unit() const
{
    return _i9on_p->_unit;
}
Node_p Manager::sheet_p(int page) const {
    auto found = _i9on_p->_sheet_p_by_tag.find(page);
    if (found != _i9on_p->_sheet_p_by_tag.end()) {
        return found->second;
    }
    return nullptr;
}

Node_p Manager::form_p(int page) const {
    auto found = _i9on_p->_form_p_by_tag.find(page);
    if (found != _i9on_p->_form_p_by_tag.end()) {
        return found->second;
    }
    return nullptr;
}

Input_p Manager::input_p(int page) const {
    return _i9on_p->input_p(page);
}

Node_v Manager::edit(
    int page,
    float h,
    float v
) {
    return Node_v();
}

Node_v Manager::view(
    std::string_view name,
    int t,
    int l,
    int c
) {
    return Node_v();
}

Node_v Manager::view(
    std::string_view name,
    int t,
    int l,
    int c,
    std::function<bool(Node_p, Node_p)>
) {
    return Node_v();
}

};

// #include "SyncTeXNode.hpp"
// #include "SyncTeXInput.hpp"
// #include "SyncTeXHandle.hpp"
// #include "SyncTeXsheet.hpp"
// #include "SyncTeXNodeForm.hpp"
// #include "SyncTeX_HV.hpp"
// #include "SyncTeXAbove.hpp"
// #include "SyncTeXBox.hpp"

namespace SyncTeXpp {


// Node_p Manager::below_of_form(int t)
// {
//     auto form_p = _form_p_by_tag[tag];
//     return form_p? form_p->below_p() : nullptr;
// }


// /*  Used when parsing the synctex file.
//  *  Read an Input record.
//  *  - parameter scanner: non nullptr scanner
//  *  - returns Status::Done on successful completions, others values otherwise.
//  */
// Status Manager::require_endl()
// {
//     return _reader_p->require_endl();
// }


// /*  Used when parsing the synctex file.
//  *  Read an Input record.
//  *  - parameter scanner: non nullptr scanner
//  *  - returns Status::Done on successful completions, others values otherwise.
//  */
// Status Manager::make_input()
// {
//     Input_p input_p = nullptr;
//     auto status = Status::ErrorBadArgument;
//     zr_s zs = {0, Status::Done};
//     if (Status::Done < (status = _reader_p->read_string(Prefix::INPUT))) {
//         return status;
//     }
//     /*  Decode the tag  */
//     int t = 0;
//     auto status = _reader_p->decode(tag);
//     if (Status::Done < status) {
//         return Status::ErrorDataUnexpected;
//     }
//     /*  The next character is a field separator, we expect one character in the buffer. */
//     if (Status::Done < (status = _reader_p->read_string(":"))) {
//         return status;
//     }
//     /*  Then we scan the file name */
//     std::string name;
//     if (Status::Done < (status = _reader_p->decode(name))) {
//         _reader_p->skip_endl(); /* Ignore this whole line */
//         return status;
//     }
//     /*  Create a node */
//     auto input_p = Input::make_p(shared_from_this(), tag, name);
//     if (input_p) {
//         _inputs_by_tag.emplace(input_p->t(), input_p);
//     }
//     /*  Prepend this input node to the input linked list of the scanner */
// #if SYNCTEX_VERBOSE
//     synctex_node_log(input);
// #endif
//     return _reader_p->require_endl(); /*  read the line termination character, if any */
// }

// /*  parse the post scriptum
//  *  Status::Done is returned on completion
//  *  a smaller error is returned otherwise */
// Status Manager::scan_post_scriptum()
// {
//     auto status = Status::Done;
//     char *endptr = nullptr;
//     /*  Scan the file until a post scriptum line is found */
//     while (true) {
//         status = read_string("Post scriptum:");
//         if (Status::Failed < status) {
//             return status;
//         }
//         if (Status::Failed == status) {
//             status = _reader_p->require_endl();
//             if (Status::EndOfData < status) {
//                 return status;
//             } else if (Status::Done < status) {
//                 return Status::Done; /*  The EndOfData is found, we have properly scanned the file */
//             }
//             continue;
//         }
//         break;
//     }
//     /*  We found the keyword switcher, advance to the next line. */
// require_endl:
//     status = _reader_p->require_endl();
//     if (Status::EndOfData < status) {
//         return status;
//     } else if (Status::Done < status) {
//         return Status::Done; /*  The EndOfData is found, we have properly scanned the file */
//     }
//     /*  Scanning the information */
//     status = read_string("Magnification:");
//     if (Status::Done == status) {
// #ifdef HAVE_SETLOCALE
//         setlocale(LC_NUMERIC, "C");
// #endif
//         status = _reader_p->decode(_unit);
// #ifdef HAVE_SETLOCALE
//         setlocale(LC_NUMERIC, loc);
// #endif
//         if (Status::Done < status) {
//             // std::cerr << "Problem with magnication." << std::endl;
//             return Status::ErrorDataPostamble;
//         }
//         if (_unit <= 0) {
//             // std::cerr << "Unexpected non positive magnication." << std::endl;
//             return Status::ErrorDataPostamble;
//         }
//         goto require_endl;
//     }
//     if (Status::EndOfData < status) {
//     report_record_problem:
//         return Status::ErrorDataPostamble; /*  echo the error */
//     }
//     status = read_string("X Offset:");
//     if (Status::Done == status) {
//         status = _reader_p->decode_united(_x_offset);
//         if (Status::Done < status) {
//             // std::cerr << "Problem with X offset in the Post Scriptum." << std::endl;
//             return Status::ErrorDataPostamble;
//         }
//         goto require_endl;
//     } else if (Status::EndOfData < status) {
//         goto report_record_problem;
//     }
//     status = read_string("Y Offset:");
//     if (Status::Done == status) {
//         status = _reader_p->decode_united(_y_offset);
//         if (Status::Done < status) {
//             // std::cerr << "Problem with Y offset in the Post Scriptum." << std::endl;
//             return Status::ErrorDataPostamble;
//         }
//         goto require_endl;
//     } else if (Status::EndOfData < status) {
//         goto report_record_problem;
//     }
//     goto require_endl;
// }

// /*  Status::Done is returned if the postamble is read
//  *  Status::Failed is returned if the postamble is not at the current location
//  *  a negative error otherwise
//  *  The postamble comprises the post scriptum section.
//  */
// Status Manager::scan_postamble()
// {
//     auto status = Status::Done;
//     if (Status::Done < !_flags.postamble && (status = read_string("Postamble:"))) {
//         return status;
//     }
// count_again:
//     if (Status::Done < (status = _reader_p->require_endl())) {
//         return Status::Done;
//     }
//     if (Status::EndOfData < (status = _reader_p->require_head("Count:"))) {
//         return status; /*  forward the error */
//     } else if (Status::Done < status) { /*  No Count record found */
//         goto count_again;
//     }
//     if (Status::EndOfData < (status = _reader_p->decode(_count))) {
//         return status;
//     }
//     if (Status::Done < (status = _reader_p->require_endl())) {
//         return Status::Done;
//     }
//     /*  Now we scan the last part of the SyncTeX file: the Post Scriptum section. */
//     return scan_post_scriptum();
// }


// Status Manager::decode(int &i1, int &i2, int &i3, bool v) {
//     Status status;
//     if (Status::Done < (status = _reader_p->decode(i1))) {
//         return status;
//     }
//     if (Status::Done < (status = v ? _reader_p->decode_v(i2) : _reader_p->decode(i2))) {
//         return status;
//     }
//     if (Status::Done < (status = v ? _reader_p->decode_v(i3) : _reader_p->decode(i2))) {
//         return status;
//     }
//     return Status::Done;
// }

// Status Manager::decode(int &i1, int &i2, bool v) {
//     Status status;
//     if (Status::Done < (status = _reader_p->decode(i1))) {
//         return status;
//     }
//     if (Status::Done < (status = v ? _reader_p->decode_v(i2) : _reader_p->decode(i2))) {
//         return status;
//     }
//     return Status::Done;
// }

// Status Manager::make_vbox(Node_p &node_p)
// {
//     return make_tlc_hv_WHD(node_p, VBox::make_p);
// }
// Status Manager::make_hbox(Node_p &node_p)
// {
//     return make_tlc_hv_WHD(node_p, HBox::make_p);
// }
// Status Manager::make_extra_box_bdry(Node_p &node_p)
// {
//     node_p = BoxBdry::make_p(shared_from_this());
//     return Status::Done;
// }
// Status Manager::make_void_vbox(Node_p &node_p)
// {
//     return make_tlc_hv_WHD(node_p, VoidVBox::make_p);
// }
// Status Manager::make_void_hbox(Node_p &node_p)
// {
//     return make_tlc_hv_WHD(node_p, VoidHBox::make_p);
// }

// void Manager::push_back(Input_p &input_p, Node_p node_p)
// {
//     if (input_p && node_p) {
//         if (input_p->t() != node_p->t()) {
//             input_p = _inputs_by_tag[node_p->t()];
//         }
//         input_p->push_back(node_p);
//     }
// }

// /**
//  * @brief Get the tags for a path represented as a string.
//  *
//  * @param name
//  * @return a possibly empty vector of ints.
//  */
// std::vector<Input_p> Manager::get_inputs(std::string_view name)
// {
//     parse(); // We must parse up to the end because input records may appear near the end.
//     std::vector<Input_p> ans;
//     auto filepath = fs::canonical(fs::path(name));
//     for (const auto& pair : _inputs_by_tag) {
//         if (fs::equivalent(filepath, pair.second->filepath())) {
//             ans.push_back(pair.second);
//         }
//     }
//     if (!ans.empty()) {
//         return ans;
//     }
//     auto filename = filepath.filename();
//     for(auto directory: {
//         _reader_p->_output_directory,
//         _reader_p->_build_directory
//     }) {
//         filepath = directory / filename;
//         for (const auto& pair : _inputs_by_tag) {
//             if (fs::equivalent(filepath, pair.second->filepath())) {
//                 ans.push_back(pair.second);
//             }
//         }
//         if (!ans.empty()) {
//             return ans;
//         }
//     }
//     return ans;
// }

// /**
//  * @brief Get the tags for a path represented as a string.
//  *
//  * @param name
//  * @return a possibly empty vector of ints.
//  */
// std::vector<int> Manager::get_tags(std::string_view name)
// {
//     auto inputs = get_inputs(name);
//     std::vector<int> ans(inputs.size());
//     std::transform(inputs.begin(), inputs.end(), ans.begin(),
//                    [](auto x) { return x->t(); });
//     return ans;
// }

// /**
//  * @brief Get the name of an input node.
//  *
//  * Corresponds to `Input:<tag>:<name>` entries in the `.synctex` file.
//  * @param scanner
//  * @param tag
//  * @return const char*
//  */
// std::string Manager::get_filename(int t)
// {
//     Input_p input_p;
//     try {
//         input_p = _inputs_by_tag.at(tag);
//     } catch (const std::out_of_range& e) {
//         return std::string();
//     }
//     return input_p? (input_p -> filename()) : (std::string());
// }

// fs::path Manager::get_filepath(int t)
// {
//      Input_p input_p;
//     try {
//         input_p = _inputs_by_tag.at(tag);
//     } catch (const std::out_of_range& e) {
//         return fs::path();
//     }
//     if (input_p) {
//         return input_p -> filepath();
//     }
//     return fs::path();
// }

// void Manager::push_back_hbox(Node_p sheet_p, Node_p node_p)
// {
//     if (sheet_p && node_p) {
//         auto page = sheet_p->t();
//         auto where = _hboxes_by_page.find(page);
//         if (where == _hboxes_by_page.end()) {
//             _hboxes_by_page[page] = {node_p};
//         } else {
//             where->second.push_back(node_p);
//         }
//     }
// }

// /**
//  *  Set the tlc of all the x nodes that are targets of
//  *  x_handle and its sibling.
//  *  Reset the target of x_handle and deletes its siblings.
//  *  child is a node that has just been parsed and is not a boundary node.
//  */
// // was _synctex_handle_make_friend_tlc
// void Manager::same_tlc(Handle_p &handle_p, Node_p node_p, Input_p &input_p)
// {
//     if (handle_p && node_p) {
//         Handle_p right_p = handle_p;
//         Node_p target_p;
//         while ((target_p = right_p->target())) {
//             target_p->set_tlc(node_p);
//             push_back(input_p, target_p);
//             if ((right_p = right_p->right_p())) {
//                 continue;
//             }
//             break;
//         }
//         handle_p->set_target(nullptr);
//         handle_p->set_right_p(nullptr);
//     }
// }

// /**
//  *  Scan sheets, forms and input records at the top level.
//  *  - returns: status
//  */

// struct SFI
// {

//     friend class Manager;
// };


// /**
//  *  Replace ref in its tree hierarchy by a single box
//  *  proxy to the contents of the associated form.
//  *  - argument ref: a ref node with no friend
//  *  - return the proxy created.
//  *  - note: Does nothing if ref is not owned.
//  *  - note: On return, ref will have no parent nor sibling.
//  *      The caller is responsible for releasing ref.
//  *  - note: this is where root proxies are created.
//  *  - note: the target of the root proxy is the content
//  *      of a form.
//  */
// Status Manager::replace_ref(Node_p ref_p)
// {
//     nr_s ns = {nullptr, Status::Done};
//     Node_p above_p;
//     if ((above_p = ref_p->above_p())) {
//         auto right_p = ref_p->set_right_p(nullptr);//TODO reset?
//         auto left_p = ref_p->left_p();
//         /*  arg_sibling != nullptr because the child of a box
//          *  is always a box boundary, not a ref. */
//         auto target_p = below_of_form(ref_p->t());
//         /*  The target is a single node (box)
//          *  with children and no siblings. */
//         if ((ns.node = __synctex_new_proxy_from_ref_to(ref_p, target_p))) {
//             /*  Insert this proxy instead of ref. */
//             left_p->set_right_p(ns.node);
//             /*  Then append the original sibling of ref. */
//             ns.node->set_right_p(right_p);
// #if defined(SYNCTEX_USE_CHARINDEX)
//             if (synctex_node_type(sibling) == synctex_node_type_box_bdry) {
//                 /*  The sibling is the last box boundary
//                  *  which may have a less accurate information */
//                 sibling->char_index = arg_sibling->char_index;
//                 sibling->line_index = arg_sibling->line_index;
//             }
// #endif
// #if SYNCTEX_DEBUG > 500
//             printf("!  Ref replacement:\n");
//             synctex_node_log(ref);
//             synctex_node_display(synctex_node_sibling(ref));
// #endif
//         } else /*  simply remove ref */ {
//             left_p->set_right_p(right_p);
//         }
//         ref_p->set_above_p(nullptr);
//     } else {
//         std::cerr <<
//             "!  Missing parent in __synctex_replace_ref. "
//             "Please report." << std::endl;
//         ns.status = Status::ErrorBadArgument;
//     }
//     return ns;
// }

// typedef synctex_node_p (*_synctex_processor_f)(synctex_node_p node);
// /**
//  *  Apply the processor f to the tree hierarchy rooted at proxy.
//  *  proxy has replaced a form ref, no children yet.
//  *  As a side effect all the hierarchy of nodes will be created.
//  */
// static SYNCTEX_INLINE Status _synctex_post_process_proxy(synctex_node_p proxy, _synctex_processor_f f)
// {
//     while (proxy) {
//         synctex_node_p next_proxy = _synctex_tree_friend(proxy);
//         synctex_node_p halt = __synctex_tree_sibling(proxy);
//         /*  if proxy is the last sibling, halt is nullptr.
//          *  Find what should be a next node,
//          *  without creating new nodes. */
//         if (!halt) {
//             synctex_node_p parent = _synctex_tree_parent(proxy);
//             halt = __synctex_tree_sibling(parent);
//             while (!halt && parent) {
//                 parent = _synctex_tree_parent(parent);
//                 halt = __synctex_tree_sibling(parent);
//             }
//         }
//         do {
// #if SYNCTEX_DEBUG > 500
//             printf("POST PROCESSING %s\n", _synctex_node_abstract(proxy));
//             {
//                 int i, j = 0;
//                 for (i = 0; i < proxy->class_->scanner->number_of_lists; ++i) {
//                     synctex_node_p N = proxy->class_->scanner->lists_of_friends[i];
//                     do {
//                         if (N == proxy) {
//                             ++j;
//                             printf("%s", _synctex_node_abstract(N));
//                         }
//                     } while ((N = _synctex_tree_friend(N)));
//                 }
//                 if (j) {
//                     printf("\nBeforehand %i match\n", j);
//                 }
//             }
// #endif
//             f(proxy);
// #if SYNCTEX_DEBUG > 500
//             {
//                 int i, j = 0;
//                 for (i = 0; i < proxy->class_->scanner->number_of_lists; ++i) {
//                     synctex_node_p N = proxy->class_->scanner->lists_of_friends[i];
//                     do {
//                         if (N == proxy) {
//                             ++j;
//                             printf("%s", _synctex_node_abstract(N));
//                         }
//                     } while ((N = _synctex_tree_friend(N)));
//                 }
//                 if (j) {
//                     printf("\n%i match\n", j);
//                 }
//             }
// #endif
//             /*  Side effect: create the hierarchy on the fly */
//             proxy = synctex_node_next(proxy); /*  Change is here */
// #if SYNCTEX_DEBUG > 500
//             if (proxy) {
//                 int i, j = 0;
//                 for (i = 0; i < proxy->class_->scanner->number_of_lists; ++i) {
//                     synctex_node_p N = proxy->class_->scanner->lists_of_friends[i];
//                     do {
//                         if (N == proxy) {
//                             ++j;
//                             printf("%s", _synctex_node_abstract(N));
//                         }
//                     } while ((N = _synctex_tree_friend(N)));
//                 }
//                 if (j) {
//                     printf("\nnext %i match\n", j);
//                 }
//             }
// #endif
//         } while (proxy && proxy != halt);
//         proxy = next_proxy;
//     }
//     return Status::Done;
// }
// /**
//  *  Replace all the form refs by root box proxies.
//  *  Create the node hierarchy and update the friends.
//  *  On entry, the refs are collected as a friend list
//  *  in either a form or a sheet
//  *  - parameter: the owning scanner
//  */
// static SYNCTEX_INLINE Status _synctex_post_process(synctex_manager_p scanner)
// {
//     Status status = Status::Done;
//     nr_s ns = {nullptr, Status::Failed};
//     /*  replace form refs inside forms by box proxies */
//     ns = _synctex_post_process_ref(scanner->ref_in_form);
//     scanner->ref_in_form = nullptr; /*  it was just released */
//     if (ns.status < status) {
//         status = ns.status;
//     }
//     /*  Create all the form proxy nodes on the fly.
//      *  ns.node is the root of the list of
//      *  newly created proxies.
//      *  There might be a problem with cascading proxies.
//      *  In order to be properly managed, the data must
//      *  be organized in the right way.
//      *  The inserted form must be defined before
//      *  the inserting one. *TeX will take care of that.   */
//     ns.status = _synctex_post_process_proxy(ns.node, &_synctex_tree_reset_friend);
//     if (ns.status < status) {
//         status = ns.status;
//     }
//     /*  replace form refs inside sheets by box proxies */
//     ns = _synctex_post_process_ref(scanner->ref_in_sheet);
//     if (ns.status < status) {
//         status = ns.status;
//     }
//     scanner->ref_in_sheet = nullptr;
// #if 0
//     {
//         int i;
//         for (i=0;i<scanner->number_of_lists;++i) {
//             synctex_node_p P = ns.node;
//             do {
//                 synctex_node_p N = scanner->lists_of_friends[i];
//                 do {
//                     if (P == N) {
//                         printf("Already registered.\n");
//                         synctex_node_display(N);
//                         break;
//                     }
//                 } while ((N = _synctex_tree_friend(N)));
//             } while((P = _synctex_tree_friend(P)));
//         }
//     }
// #endif
//     ns.status = _synctex_post_process_proxy(ns.node, &__synctex_proxy_make_friend_and_next_hbox);
//     if (ns.status < status) {
//         status = ns.status;
//     }
//     return status;
// }


// static synctex_node_p _synctex_display_query_v2(synctex_node_p target, int tag, int line, synctex_bool_t exclude_box)
// {
//     synctex_node_p first_handle = NULL;
//     /*  Search the first match */
//     if (target == NULL) {
//         return first_handle;
//     }
//     do {
//         int page;
//         if ((exclude_box && _synctex_node_is_box(target)) || (tag != synctex_node_tag(target)) || (line != synctex_node_line(target))) {
//             continue;
//         }
//         /*  We found a first match, create
//          *  a result handle targeting that candidate. */
//         first_handle = _synctex_new_handle_with_target(target);
//         if (first_handle == NULL) {
//             return first_handle;
//         }
//         /*  target is either a node,
//          *  or a proxy to some node, in which case,
//          *  the target's target belongs to a form,
//          *  not a sheet. */
//         page = synctex_node_page(target);
//         /*  Now create all the other results  */
//         while ((target = _synctex_tree_friend(target))) {
//             synctex_node_p result = NULL;
//             if ((exclude_box && _synctex_node_is_box(target)) || (tag != synctex_node_tag(target)) || (line != synctex_node_line(target))) {
//                 continue;
//             }
//             /*  Another match, same page number ? */
//             result = _synctex_new_handle_with_target(target);
//             if (NULL == result) {
//                 return first_handle;
//             }
//             /*  is it the same page number ? */
//             if (synctex_node_page(target) == page) {
//                 __synctex_tree_set_child(result, first_handle);
//                 first_handle = result;
//             } else {
//                 /*  We have 2 page numbers involved */
//                 __synctex_tree_set_sibling(first_handle, result);
//                 while ((target = _synctex_tree_friend(target))) {
//                     synctex_node_p same_page_node;
//                     if ((exclude_box && _synctex_node_is_box(target)) || (tag != synctex_node_tag(target)) || (line != synctex_node_line(target))) {
//                         continue;
//                     }
//                     /*  New match found, which page? */
//                     result = _synctex_new_handle_with_target(target);
//                     if (NULL == result) {
//                         return first_handle;
//                     }
//                     same_page_node = first_handle;
//                     page = synctex_node_page(target);
//                     /*  Find a result with the same page number */;
//                     do {
//                         if (_synctex_node_target_page(same_page_node) == page) {
//                             /* Insert result between same_page_node and its child */
//                             _synctex_tree_set_child(result, _synctex_tree_set_child(same_page_node, result));
//                         } else if ((same_page_node = __synctex_tree_sibling(same_page_node))) {
//                             continue;
//                         } else {
//                             /*  This is a new page number */
//                             __synctex_tree_set_sibling(result, first_handle);
//                             first_handle = result;
//                         }
//                         break;
//                     } while (synctex_YES);
//                 }
//                 return first_handle;
//             }
//         }
//     } while ((target = _synctex_tree_friend(target)));
//     return first_handle;
// }

// /**
//  *  The best is the one with the smallest area.
//  *  The area is width*height where width and height may be big.
//  *  So there is a real risk of overflow if we stick with ints.
//  */
// Node_p Manager::smallest_container_v2(Node_p node1_p, Node_p node2_p)
// {
//     if (!node1_p) {
//         return node2_p;
//     }
//     if (!node2_p) {
//         return node1_p;
//     }
//     auto width1 = std::abs(node1_p->w());
//     auto width2 = std::abs(node2_p->w());
//     auto total_height1 = std::abs(node1_p->d()) + std::abs(node1_p->g());
//     auto total_height2 = std::abs(node2_p->d()) + std::abs(node2_p->g());
//     auto area1 = (long long)width1 * total_height1;
//     auto area2 = (long long)width2 * total_height2;
//     if (area1 < area2) {
//         return node1_p;
//     }
//     if (area1 > area2) {
//         return node2_p;
//     }
//     if (width_1 > width_2) {
//         return node1_p;
//     }
//     if (width_1 < width_2) {
//         return node2_p;
//     }
//      return node1_p;
// }

// Node_v Manager::view(std::string_view name, int l, int c)
// {
//     parse();
//     Node_v ans;
//     for (const auto input_p: get_inputs(name)) {
//         Node_v ans_box;
//         Node_v ans_no_box;
//         int below = 0;
//         int here = 0;
//         int above = 0;
//         input_p->iterate([below, here, above](int i, bool &stop) {
//             if (i<line) {
//                 below = i;
//             } else if (i>line) {
//                 above = i;
//                 stop = true;
//             } else {
//                 here = i; // == line
//                 stop = true;
//             }
//         })
//         if (here==0) {
//             // Replace with the closest line
//             if (above > line && above-line < line - below) {
//                 here = above;
//             } else {
//                 here = below;
//             }
//         }
//         input_p->iterate(here, [ans_box, ans_no_box](Node_p p) {
//             if (p->isBox()) {
//                 ans_box.push_back(p);
//             } else {
//                 ans_no_box.push_back(p);
//             }
//         });
//         // IN PROGRESS
//         if (ans_no_box.empty()) {
//             ans.insert(
//                 ans.end(),
//                 std::make_move_iterator(ans_box.begin()),
//                 std::make_move_iterator(ans_box.end())
//             );
//         } else {
//             ans.insert(
//                 ans.end(),
//                 std::make_move_iterator(ans_no_box.begin()),
//                 std::make_move_iterator(ans_no_box.end())
//             );
//         }
//         // IN PROGRESS
//     }
//     return ans;
// }

// Node_v Manager::view(std::string_view name, int l, int c, std::function<bool(Node_p, Node_p)> compare)
// {
//     auto ans = view(name, line, column)
//     set::sort(and.begin(), and.end(), compare);
//     return ans;
// }
//     void foreach_hbox(int page, std::function<void(Node_p)>);

// void Manager::foreach_hbox(int page, std::function<void(Node_p)> f)
// {
//     auto what = _hboxes_by_page.find(page);
//     if (what == _hboxes_by_page.end()) {
//         return;
//     }
//     for (const auto p: what.second) {
//         f(p);
//     }
// }
// void Manager::foreach_hbox(int page, std::function<void(Node_p, bool &)> f)
// {
//     auto what = _hboxes_by_page.find(page);
//     if (what == _hboxes_by_page.end()) {
//         return;
//     }
//     bool stop = false;
//     for (const auto p: what.second) {
//         f(p, stop);
//         if (stop) break;
//     }
// }

// Node_v Manager::edit(int page, float h, float v)
// {
//     parse();
    
//     Node_v ans;
//     /*  Find the proper sheet */
//     Sheet_p sheet_p = get_sheet(page);
//     if (!sheet_p) {
//         return ans;
//     }
    
//     /*  Now sheet_p points to the sheet node with proper page number. */
//     /*  Now that scanner has been initialized, we can convert
//         *  the given point to scanner integer coordinates */
//     hv_s hit = {
//         (h - scanner->x_offset) / scanner->unit,
//         (v - scanner->y_offset) / scanner->unit
//     };
//     Node_p node_p = nullptr;
//     lr_nd_s nds = {{nullptr, 0}, {nullptr, 0}};

//     /*  At first, we browse all the horizontal boxes of the sheet
//         *  until we find one containing the hit point. */
    
//     using Ni = std::pair<Node_p, int>

//     std::vector<Ni> hboxes;
//     foreach_hbox(page, [hboxes](Node_p p){
//         hboxes.push_back(Ni(p, std::abs(p->distance(hit))));
//     });
//     std::sort(hboxes.begin(), hboxes.end(), [](Ni left, Ni right) {
//         return left.second < right.second;
//     });
    
//     if ((node_p = sheet_p->next_hbox())) {
//         do {
//             if (node_p->contains(hit)) {
//                 /*  Maybe the hit point belongs to a contained vertical box.
//                     *  This is the most likely situation.
//                     */
//                 Node_p next_p = node_p;
//                 /*  This trick is for catching overlapping boxes */
//                 while ((next_p = next_p->next_hbox())) {
//                     if (next_p->contains(hit)) {
//                         node_p = smallest_container_v2(next_p, node_p);
//                     }
//                 }
//                 /*  node is the smallest horizontal box that contains hit,
//                 *  unless there is no hbox at all.
//                 */
//                 node_p = node_p->closest_container_below(hit);
//                 nds = node_p->eq_get_closest_below_in_box_v2(hit);
//             end:
//                 if (nds.right.node_p && nds.left.node_p) {
//                     if ((LR::t(nds.right.node_p) != LR::t(nds.left.node_p))
//                         || (LR::l(nds.right.node_p) != LR::l(nds.left.node_p))
//                         || (LR::c(nds.right.node_p) != LR::c(nds.left.node_p))) {
//                         if (LR::l(nds.right.node_p) < LR::l(nds.left.node_p)) {
//                             node_p = nds.right.node_p;
//                             nds.right.node_p = nds.left.node_p;
//                             nds.left.node_p = node_p;
//                         } else if (LR::l(nds.right.node_p) == LR::l(nds.left.node_p)) {
//                             if (nds.left.i > nds.right.i) {
//                                 node_p = nds.right.node_p;
//                                 nds.right.node_p = nds.left.node_p;
//                                 nds.left.node_p = node_p;
//                             }
//                         }
//                         if ((node_p = _synctex_new_handle_with_target(nds.left.node_p))) {
//                             synctex_node_p other_handle;
//                             if ((other_handle = _synctex_new_handle_with_target(nds.right.node_p))) {
//                                 _synctex_tree_set_sibling(node_p, other_handle);
//                                 return _synctex_iterator_new(node_p, 2);
//                             }
//                             return _synctex_iterator_new(node_p, 1);
//                         }
//                         return ans;
//                     }
//                     /*  both nodes have the same input coordinates
//                         *  We choose the one closest to the hit point  */
//                     if (nds.left.i > nds.right.i) {
//                         nds.left.node_p = nds.right.node_p;
//                     }
//                     nds.right.node_p = nullptr;
//                 } else if (nds.right.node_p) {
//                     nds.left = nds.right;
//                 } else if (!nds.left.node_p) {
//                     nds.left.node_p = node_p;
//                 }
//                 ans.push_back(node_p);
//                 return ans;
//             }
//         } while ((node_p = node_p->next_hbox()));
//         /*  All the horizontal boxes have been tested,
//             *  None of them contains the hit point.
//             */
//     }
//     /*  We are not lucky,
//         *  we test absolutely all the node
//         *  to find the closest... */
//     if ((node_p = sheet_p->below_p())) {
// #if defined(SYNCTEX_DEBUG)
//         printf("--- We are not lucky\n");
// #endif
//         nds.left = node_p->_closest_deep_below_v2(hit);
// #if defined(SYNCTEX_DEBUG)
//         printf("Edit query best: %i\n", nds.left.i);
// #endif
//         goto end;
//     }
// }

};

#if 0

synctex_manager_p synctex_scanner_new()
{
    synctex_manager_p scanner = (synctex_manager_p)_synctex_malloc(sizeof(_synctex_scanner_s));
    if (scanner) {
        if (!(_reader_p = _synctex_malloc(sizeof(_synctex_reader_s)))) {
            _synctex_free(scanner);
            return nullptr;
        }

#define DEFINE_synctex_scanner_class(NAME)                                                                                                                     \
    scanner->class_[synctex_node_type_##NAME] = _synctex_class_##NAME;                                                                                         \
    (scanner->class_[synctex_node_type_##NAME]).scanner = scanner

        DEFINE_synctex_scanner_class(input);
        DEFINE_synctex_scanner_class(sheet);
        DEFINE_synctex_scanner_class(form);
        DEFINE_synctex_scanner_class(hbox);
        DEFINE_synctex_scanner_class(void_hbox);
        DEFINE_synctex_scanner_class(vbox);
        DEFINE_synctex_scanner_class(void_vbox);
        DEFINE_synctex_scanner_class(kern);
        DEFINE_synctex_scanner_class(glue);
        DEFINE_synctex_scanner_class(rule);
        DEFINE_synctex_scanner_class(math);
        DEFINE_synctex_scanner_class(boundary);
        DEFINE_synctex_scanner_class(box_bdry);
        DEFINE_synctex_scanner_class(ref);
        DEFINE_synctex_scanner_class(proxy_hbox);
        DEFINE_synctex_scanner_class(proxy_vbox);
        DEFINE_synctex_scanner_class(proxy);
        DEFINE_synctex_scanner_class(proxy_last);
        DEFINE_synctex_scanner_class(handle);
        /*  set up the lists of friends */
        scanner->number_of_lists = 1024;
        scanner->lists_of_friends = (synctex_node_r)_synctex_malloc(scanner->number_of_lists * sizeof(synctex_node_p));
        if (nullptr == scanner->lists_of_friends) {
            std::cerr << "malloc:2" << std::endl;
            return nullptr;
        }
        scanner->display_switcher = 100;
        scanner->display_prompt = (char *)_synctex_display_prompt + strlen(_synctex_display_prompt) - 1;
    }
    return scanner;
}
/*  The scanner destructor
 */

#undef SYNCTEX_FILE

/*  Manager accessors.
 */

void synctex_scanner_display()
{
    if (nullptr == scanner) {
        return;
    }
    printf("The scanner:\noutput:%s\noutput_fmt:%s\nversion:%i\n", scanner->reader->output, scanner->output_fmt, scanner->version);
    printf("pre_unit:%i\nx_offset:%i\ny_offset:%i\n", scanner->pre_unit, scanner->pre_x_offset, scanner->pre_y_offset);
    printf("count:%i\npost_magnification:%f\npost_x_offset:%f\npost_y_offset:%f\n", scanner->count, scanner->unit, scanner->x_offset, scanner->y_offset);
    printf("The input:\n");
    synctex_node_display(scanner->input);
    if (scanner->count < 1000) {
        printf("The sheets:\n");
        synctex_node_display(scanner->sheet);
        printf("The friends:\n");
        if (scanner->lists_of_friends) {
            int i = scanner->number_of_lists;
            synctex_node_p node_p;
            while (i--) {
                printf("Friend index:%i\n", i);
                node_p = (scanner->lists_of_friends)[i];
                while (node_p) {
                    printf("%s:%i,%i\n", synctex_node_isa(node_p), _synctex_data_t(node_p), _synctex_data_l(node_p));
                    node_p = _synctex_tree_friend(node_p);
                }
            }
        }
    } else {
        printf("SyncTeX Warning: Too many objects\n");
    }
}

#if defined(SYNCTEX_USE_CHARINDEX)
synctex_node_p synctex_scanner_handle()
{
    return scanner ? scanner->handle : nullptr;
}
#endif

#ifdef SYNCTEX_NOTHING
#pragma mark -
#pragma mark SYNCTEX_DISPLAY
#endif

int synctex_scanner_display_switcher(synctex_manager_p scanR)
{
    return scanR->display_switcher;
}
void synctex_scanner_set_display_switcher(synctex_manager_p scanR, int switcher)
{
    scanR->display_switcher = switcher;
}
static const char *const _synctex_display_prompt = "................................";

static char *_synctex_scanner_display_prompt_down(synctex_manager_p scanR)
{
    if (scanR->display_prompt > _synctex_display_prompt) {
        --scanR->display_prompt;
    }
    return scanR->display_prompt;
}
static char *_synctex_scanner_display_prompt_up(synctex_manager_p scanR)
{
    if (scanR->display_prompt + 1 < _synctex_display_prompt + strlen(_synctex_display_prompt)) {
        ++scanR->display_prompt;
    }
    return scanR->display_prompt;
}


#endif
