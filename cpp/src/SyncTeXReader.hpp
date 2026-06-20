/**
 * SyncTeXReader.hpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

// #pragma once

/**
 * @brief Reader of the SyncTeX data.
 * 
 * Used by a Scanner instance.
 */

#ifndef SYNCTEX_READER_HPP
#define SYNCTEX_READER_HPP

#include <memory>
#include <vector>
#include <limits>
#include <string>
#include <string_view>
#include <map>
#include <cstddef>
#include <filesystem>
namespace fs = std::filesystem;

#include <zlib.h>

#include "SyncTeXBufferChef.hpp"

/**
 * @brief Data structure for a file reader
 *
 */

#include <string>

namespace SyncTeXpp {

class Scanner;

enum class IO {
    r,
    rb,
    a,
    ab
};

const std::map<IO, std::string> IONames = {
    {IO::r,  "r" },
    {IO::rb, "rb"},
    {IO::a,  "r" },
    {IO::ab, "rb"}
};


class Reader;
using Reader_p = std::shared_ptr<Reader>;

/**
 * @brief Reader structure
 *
 * It can read a synctex data file but it does not know
 * what to do with the result.
 */

class Reader: public BufferChef
{
    public:
// IO mode types
using io_mode_t = unsigned int;

// Suffixes for SyncTeX files
/**
 * @brief Extension of uncompressed synctex file
 */
    const std::string_view EXT_SYNCTEX = ".synctex";
/**
 * @brief Extension of compressed synctex file
 */
    const std::string_view EXT_SYNCTEX_GZ = ".synctex.gz";

    enum class IOMask : io_mode_t {
        Append   = 1 << 0,
        GZ       = 1 << 1,
    };

    private:
    /** The (possibly compressed) file */
    gzFile _file;
    /** The output base */
    std::string _jobname;
    /** The output directory */
    fs::path _output_directory;
    /** The build directory */
    fs::path _build_directory;  
    /** The synctex file */
    fs::path _synctex;
    /** mode */
    IO _io_mode;

    public:
    Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;
    Reader(Reader&&) = delete;
    Reader& operator=(Reader&&) = delete;
    Reader() = default;
    
    /*
    *  Returns true on success, returns false on failure
    %  (the reader is nullptr, the output is nullptr or empty, there is a malloc error).
    */
    protected:
    Reader(
        fs::path output,
        fs::path build_directory
    ): BufferChef(BufferChef::Size::capacity) {
        init_with_output_file(output, build_directory);
    }
    public:
    static Reader_p make_p(
        fs::path output,
        fs::path build_directory = fs::path()
    ) {
        return std::shared_ptr<Reader>(new Reader(
            output, build_directory
        ));
    }
    virtual ~Reader() {
        if (_file) gzclose(_file);
    }

    /**
     * @brief Initialize the receiver with the given arguments.
     * 
     * Opens the synctex data file from the output file.
     * Where is this data file located ?
     * * in either <jobname>.synctex or <jobname>.synctex.gz where
     *   <jobname> is the last past component of _output without
     *   the last path extension (in general pdf)
     * * in the same directory as the output
     * * in the build directory
     * 
     * @param output path of the pdf output file
     * @param build_directory possible build directory
     * @return Status 
     */
    Status init_with_output_file(
        fs::path output,
        fs::path build_directory
    );

    Restore_f save() override {
        if (_file) {
            z_off_t pos = gztell(_file);
            return [this, pos](){
                if (_file) {
                    z_off_t actual = gztell(_file);
                    // If the current file position has changed
                    // the data buffer has been filled
                    // 
                    if (pos<actual) gzseek(_file, pos, SEEK_SET);
                }
            };
        }
        return BufferChef::save();
    }

    int fill(char *ptr, int len) override {
        if (_file && ptr) {
            return gzread(_file, static_cast<void*>(ptr), len);
        }
        return 0;
    }
    friend class Scanner;

    fs::path output_directory() const { return _output_directory; }

    friend class Manager;
    SYNCTEX_TEST_VISITOR

};

using Reader_p = std::shared_ptr<Reader>;

inline Status Reader::init_with_output_file(
    fs::path output,
    fs::path build_directory
) {
    _build_directory = build_directory;
    if (_file) {
        gzclose(_file);
        _file = Z_NULL;
    }
    _jobname = output.filename().stem();
    _output_directory = output.parent_path();
    // now open the synctex file
    auto add_quotes = (_jobname.find(' ') != std::string::npos)
        && (_jobname.find('"') == std::string::npos);
    while(true) {
        for (auto directory: {_output_directory, _build_directory}) {
            for (auto extension: {EXT_SYNCTEX, EXT_SYNCTEX_GZ}) {
                fs::path candidate;
                if (add_quotes) {
                    std::string base = _jobname;
                    std::string quoted_base = "\"" + base + "\"";
                    candidate = directory / (quoted_base + std::string(extension));
                    if ((_file = gzopen(candidate.c_str(), "rb"))) {
                        gzclose(_file);
                        _file = Z_NULL;
                        auto destination = directory / (base + std::string(extension));
                        try {
                            fs::rename(candidate, destination);
                            if ((_file = gzopen(destination.c_str(), "rb"))) {
                                candidate = destination;
                            }
                        } catch (const fs::filesystem_error& e) {
                            _file = gzopen(candidate.c_str(), "rb");
                        }
                        if (_file) {
                            _synctex = candidate;
                            reset();
                            return Status::Done;
                        }
                    }
                } else {
                    candidate = directory / (_jobname + std::string(extension));
                    if ((_file = gzopen(candidate.c_str(), "rb"))) {
                        _synctex = candidate;
                        reset();
                        return Status::Done;
                    }
                }
            }
        }
        if (!add_quotes) {
            break;
        }
        add_quotes = false;
    }
    return Status::ErrorFS;
}

};
#endif // SYNCTEX_READER_HPP




#if defined(SYNCTEX_USE_HANDLE)
static SYNCTEX_INLINE void __synctex_scanner_remove_handle_to(synctex_node_p node)
{
    synctex_node_p arg_sibling = nullptr;
    synctex_node_p handle = node->class_->scanner->handle;
    while (handle) {
        synctex_node_p sibling;
        if (node == _synctex_tree_target(handle)) {
            sibling = __synctex_tree_reset_sibling(handle);
            if (arg_sibling) {
                __synctex_tree_set_sibling(arg_sibling, sibling);
            } else {
                node->class_->scanner->handle = sibling;
            }
            _synctex_node_free(handle);
            break;
        } else {
            sibling = __synctex_tree_sibling(handle);
        }
        arg_sibling = handle;
        handle = sibling;
    }
}
static SYNCTEX_INLINE void __synctex_scanner_register_handle_to(synctex_node_p node)
{
    synctex_node_p NNN = _synctex_new_handle_with_target(node);
    __synctex_tree_set_sibling(NNN, node->class_->scanner->handle);
    node->class_->scanner->handle = NNN;
}
#endif

#if SYNCTEX_USE_NODE_COUNT > 10
static SYNCTEX_INLINE void _synctex_did_new(synctex_node_p node)
{
    printf("NODE CREATED # %i, %s, %p\n", (node->class_->scanner->node_count)++, synctex_node_isa(node), node);
}
static SYNCTEX_INLINE void _synctex_will_free(synctex_node_p node)
{
    printf("NODE DELETED # %i, %s, %p\n", --(node->class_->scanner->node_count), synctex_node_isa(node), node);
}
#endif


#if 0

/**
 *  The returned proxy will be the child or a sibling of source.
 *  The returned proxy has no parent, child nor sibling.
 *  Used only by __synctex_replace_ref.
 *  argument to_node: a box, not a proxy nor anything else.
 */
static SYNCTEX_INLINE synctex_node_p __synctex_new_proxy_from_ref_to(synctex_node_p ref, synctex_node_p to_node)
{
    synctex_node_p proxy = nullptr;
    if (!ref || !to_node) {
        return nullptr;
    }
    switch (synctex_node_type(to_node)) {
    case synctex_node_type_vbox:
        proxy = _synctex_new_proxy_vbox(ref->class_->scanner);
        break;
    case synctex_node_type_hbox:
        proxy = _synctex_new_proxy_hbox(ref->class_->scanner);
        break;
    default:
        std::cerr << "!  __synctex_new_proxy_from_ref_to. Unexpected form child (%s). Please report.", synctex_node_isa(to_node) << std::endl;
        return nullptr;
    }
    if (!proxy) {
        std::cerr << "!  __synctex_new_proxy_from_ref_to. Internal error. Please report." << std::endl;
        return nullptr;
    }
    _synctex_data_set_h(proxy, _synctex_data_h(ref));
    _synctex_data_set_v(proxy, _synctex_data_v(ref) - _synctex_data_g(to_node));
    _synctex_tree_set_target(proxy, to_node);
#if defined(SYNCTEX_USE_CHARINDEX)
    proxy->line_index = to_node ? to_node->line_index : 0;
    proxy->char_index = to_node ? to_node->char_index : 0;
#endif
    return proxy;
}
static char *_synctex_node_abstract(synctex_node_p node);
static SYNCTEX_INLINE synctex_node_p synctex_tree_set_friend(synctex_node_p node, synctex_node_p new_friend)
{
#if SYNCTEX_DEBUG
    synctex_node_p F = new_friend;
    while (F) {
        if (node == F) {
            printf("THIS IS AN Error\n");
            F = new_friend;
            while (F) {
                printf("%s\n", _synctex_node_abstract(F));
                if (node == F) {
                    return nullptr;
                }
                F = _synctex_tree_friend(F);
            }
            return nullptr;
        }
        F = _synctex_tree_friend(F);
    }
#endif
    return new_friend ? _synctex_tree_set_friend(node, new_friend) : _synctex_tree_reset_friend(node);
}
/**
 *  All proxies have tlc attributes, on behalf of their target.
 *  The purpose is to register all af them.
 *  - argument node: is the proxy, must not be nullptr
 */
static SYNCTEX_INLINE synctex_node_p __synctex_proxy_make_friend_and_next_hbox(synctex_node_p node)
{
    synctex_node_p old = nullptr;
    synctex_node_p target = _synctex_tree_target(node);
    if (target) {
        int i = _synctex_data_t(target) + _synctex_data_l(target);
        old = __synctex_node_make_friend(node, i);
    } else {
        old = __synctex_tree_reset_friend(node);
    }
    if (synctex_node_type(node) == synctex_node_type_proxy_hbox) {
        synctex_node_p sheet = synctex_node_parent_sheet(node);
        if (sheet) {
            _synctex_tree_set_next_hbox(node, _synctex_tree_next_hbox(sheet));
            _synctex_tree_set_next_hbox(sheet, node);
        }
    }
    return old;
}
/**
 *  Register a node which have tag, line and column.
 *  - argument node: the node
 */
static SYNCTEX_INLINE synctex_node_p __synctex_node_make_friend_tlc(synctex_node_p node)
{
    int i = synctex_node_t(node) + synctex_node_l(node);
    return __synctex_node_make_friend(node, i);
}
/**
 *  Register a node which have tag, line and column.
 *  Does nothing if the argument is nullptr.
 *  Calls __synctex_node_make_friend_tlc.
 *  - argument node: the node
 */
static SYNCTEX_INLINE void _synctex_node_make_friend_tlc(synctex_node_p node)
{
    if (node) {
        __synctex_node_make_friend_tlc(node);
    }
}
static synctex_node_p _synctex_node_set_child(synctex_node_p node, synctex_node_p new_child);

/**
 *  The (first) child of the node, if any, nullptr otherwise.
 *  At parse time, non void box nodes have children.
 *  All other nodes have no children.
 *  In order to support pdf forms, proxies are created
 *  to place form nodes at real locations.
 *  Ref nodes are replaced by root proxies targeting
 *  form contents. If root proxies have no children,
 *  they are created on the fly as proxies to the
 *  children of the targeted box.
 *  As such, proxies created here are targeting a
 *  node that belongs to a form.
 *  This is the only place where child proxies are created.
 */
synctex_node_p synctex_node_child(synctex_node_p node)
{
    synctex_node_p child = nullptr;
    synctex_node_p target = nullptr;
    if ((child = _synctex_tree_child(node))) {
        return child;
    } else if ((target = _synctex_tree_target(node))) {
        if ((child = synctex_node_child(target))) {
            /*  This is a proxy with no child
             *  which target does have a child. */
            nnr_s nns = _synctex_new_child_proxies_to(node, child);
            if (nns.first) {
                _synctex_node_set_child(node, nns.first);
                return nns.first;
            } else {
                std::cerr << "!  synctex_node_child. Internal inconsistency. Please report." << std::endl;
            }
        }
    }
    return nullptr;
}

/*  The last child of the given node, or nullptr.
 */
synctex_node_p synctex_node_last_child(synctex_node_p node)
{
    return _synctex_tree_last(node);
}
/**
 *  All nodes siblings are properly set up at parse time
 *  except for non root proxies.
 */
synctex_node_p synctex_node_sibling(synctex_node_p node)
{
    return node ? __synctex_tree_sibling(node) : nullptr;
}
/**
 *  All the _synctex_tree_... methods refer to the tree model.
 *  __synctex_tree_... methods are low level.
 */
/**
 *  Replace the sibling.
 *  Connect to the arg_sibling of the new_sibling if relevant.
 *  - returns the old sibling.
 *  The caller is responsible for releasing the old sibling.
 *  The bound to the parent is managed below.
 */
static SYNCTEX_INLINE synctex_node_p _synctex_tree_set_sibling(synctex_node_p node, synctex_node_p new_sibling)
{
    if (node == new_sibling) {
        printf("BOF\n");
    }
    synctex_node_p old = node ? __synctex_tree_set_sibling(node, new_sibling) : nullptr;
    _synctex_tree_set_arg_sibling(new_sibling, node);
    return old;
}
/**
 *  Replace the sibling.
 *  Set the parent of the new sibling (and further siblings)
 *  to the parent of the receiver.
 *  Also set the last sibling of parent.
 *  - argument new_sibling: must not be nullptr.
 *  - returns the old sibling.
 *  The caller is responsible for releasing the old sibling.
 */
static synctex_node_p _synctex_node_set_sibling(synctex_node_p node, synctex_node_p new_sibling)
{
    if (node && new_sibling) {
        synctex_node_p old = _synctex_tree_set_sibling(node, new_sibling);
        if (_synctex_tree_has_parent(node)) {
            synctex_node_p parent = __synctex_tree_parent(node);
            if (parent) {
                synctex_node_p N = new_sibling;
                while (synctex_YES) {
                    if (_synctex_tree_has_parent(N)) {
                        __synctex_tree_set_parent(N, parent);
                        _synctex_tree_set_last(parent, N);
                        N = __synctex_tree_sibling(N);
                        continue;
                    } else if (N) {
                        std::cerr <<
                            "!  synctex_node_sibling. "
                            "Internal inconsistency. "
                            "Please report." << std::endl;
                    }
                    break;
                }
            }
        }
        return old;
    }
    return nullptr;
}
/**
 *  The next nodes corresponds to a deep first tree traversal.
 *  Does not create child proxies as side effect contrary to
 *  the synctex_node_next method below.
 *  May loop infinitely many times if the tree
 *  is not properly built (contains loops).
 */
static SYNCTEX_INLINE synctex_node_p _synctex_node_sibling_or_parents(synctex_node_p node)
{
    while (node) {
        synctex_node_p N;
        if ((N = __synctex_tree_sibling(node))) {
            return N;
        } else if ((node = _synctex_tree_parent(node))) {
            if (synctex_node_type(node) == synctex_node_type_sheet) { /*  EXC_BAD_ACCESS? */
                return nullptr;
            } else if (synctex_node_type(node) == synctex_node_type_form) {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }
    return nullptr;
}
/**
 *  The next nodes corresponds to a deep first tree traversal.
 *  Creates child proxies as side effect.
 *  May loop infinitely many times if the tree
 *  is not properly built (contains loops).
 */
synctex_node_p synctex_node_next(synctex_node_p node)
{
    synctex_node_p N = synctex_node_child(node);
    if (N) {
        return N;
    }
    return _synctex_node_sibling_or_parents(node);
}
/**
 *  The node which argument is the sibling.
 *  - return: nullptr if the argument has no parent or
 *      is the first child of its parent.
 *  - Input nodes have no arg siblings
 */
synctex_node_p synctex_node_arg_sibling(synctex_node_p node)
{
#if 1
    return _synctex_tree_arg_sibling(node);
#else
    synctex_node_p N = _synctex_tree_parent(node);
    if ((N = _synctex_tree_child(N))) {
        do {
            synctex_node_p NN = __synctex_tree_sibling(N);
            if (NN == node) {
                return N;
            }
            N = NN;
        } while (N);
    }
    return N;
#endif
}

/*  Public node accessor: the type  */
synctex_node_type_t synctex_node_type(synctex_node_p node)
{
    return node ? node->class_->type : synctex_node_type_none;
}

/*  Public node accessor: the type  */
synctex_node_type_t synctex_node_target_type(synctex_node_p node)
{
    synctex_node_p target = _synctex_tree_target(node);
    if (target) {
        return (((target)->class_))->type;
    } else if (node) {
        return (((node)->class_))->type;
    }
    return synctex_node_type_none;
}

/*  Public node accessor: the human readable type  */
const char *synctex_node_isa(synctex_node_p node)
{
    static const char *isa[synctex_node_number_of_types] = {"Not a node", "input",     "sheet",      "form",       "ref",        "vbox",  "void vbox",
                                                            "hbox",       "void hbox", "kern",       "glue",       "rule",       "math",  "boundary",
                                                            "box_bdry",   "proxy",     "last proxy", "vbox proxy", "hbox proxy", "handle"};
    return isa[synctex_node_type(node)];
}


/*  Public node logger  */
void synctex_node_log(synctex_node_p node)
{
    SYNCTEX_MSG_SEND(node, log);
}

static void _synctex_log_input(synctex_node_p node)
{
    if (node) {
        printf("%s:%i,%s(%i)\n", synctex_node_isa(node), _synctex_data_t(node), _synctex_data_name(node), _synctex_data_l(node));
        printf("SELF:%p\n", (void *)node);
        printf("    SIBLING:%p\n", (void *)__synctex_tree_sibling(node));
    }
}

static void _synctex_log_sheet(synctex_node_p node)
{
    if (node) {
        printf("%s:%i", synctex_node_isa(node), _synctex_data_page(node));
        SYNCTEX_PRINT_CHARINDEX_NL;
        printf("SELF:%p\n", (void *)node);
        printf("    SIBLING:%p\n", (void *)__synctex_tree_sibling(node));
        printf("    PARENT:%p\n", (void *)_synctex_tree_parent(node));
        printf("    CHILD:%p\n", (void *)_synctex_tree_child(node));
        printf("    LEFT:%p\n", (void *)_synctex_tree_friend(node));
        printf("    NEXT_hbox:%p\n", (void *)_synctex_tree_next_hbox(node));
    }
}

static void _synctex_log_form(synctex_node_p node)
{
    if (node) {
        printf("%s:%i", synctex_node_isa(node), _synctex_data_t(node));
        SYNCTEX_PRINT_CHARINDEX_NL;
        printf("SELF:%p\n", (void *)node);
        printf("    SIBLING:%p\n", (void *)__synctex_tree_sibling(node));
        printf("    PARENT:%p\n", (void *)_synctex_tree_parent(node));
        printf("    CHILD:%p\n", (void *)_synctex_tree_child(node));
        printf("    LEFT:%p\n", (void *)_synctex_tree_friend(node));
    }
}

static void _synctex_log_ref(synctex_node_p node)
{
    if (node) {
        printf("%s:%i:%i,%i", synctex_node_isa(node), _synctex_data_t(node), _synctex_data_h(node), _synctex_data_v(node));
        SYNCTEX_PRINT_CHARINDEX_NL;
        printf("SELF:%p\n", (void *)node);
        printf("    SIBLING:%p\n", (void *)__synctex_tree_sibling(node));
        printf("    PARENT:%p\n", (void *)_synctex_tree_parent(node));
    }
}

static void _synctex_log_tlchv_node(synctex_node_p node)
{
    if (node) {
        printf("%s:%i,%i,%i:%i,%i",
               synctex_node_isa(node),
               _synctex_data_t(node),
               _synctex_data_l(node),
               _synctex_data_c(node),
               _synctex_data_h(node),
               _synctex_data_v(node));
        SYNCTEX_PRINT_CHARINDEX_NL;
        printf("SELF:%p\n", (void *)node);
        printf("    SIBLING:%p\n", (void *)__synctex_tree_sibling(node));
        printf("    PARENT:%p\n", (void *)_synctex_tree_parent(node));
        printf("    CHILD:%p\n", (void *)_synctex_tree_child(node));
        printf("    LEFT:%p\n", (void *)_synctex_tree_friend(node));
    }
}

static void _synctex_log_kern_node(synctex_node_p node)
{
    if (node) {
        printf("%s:%i,%i,%i:%i,%i:%i",
               synctex_node_isa(node),
               _synctex_data_t(node),
               _synctex_data_l(node),
               _synctex_data_c(node),
               _synctex_data_h(node),
               _synctex_data_v(node),
               node_p->w());
        SYNCTEX_PRINT_CHARINDEX_NL;
        printf("SELF:%p\n", (void *)node);
        printf("    SIBLING:%p\n", (void *)__synctex_tree_sibling(node));
        printf("    PARENT:%p\n", (void *)_synctex_tree_parent(node));
        printf("    CHILD:%p\n", (void *)_synctex_tree_child(node));
        printf("    LEFT:%p\n", (void *)_synctex_tree_friend(node));
    }
}

static void _synctex_log_rule(synctex_node_p node)
{
    if (node) {
        printf("%s:%i,%i,%i:%i,%i",
               synctex_node_isa(node),
               _synctex_data_t(node),
               _synctex_data_l(node),
               _synctex_data_c(node),
               _synctex_data_h(node),
               _synctex_data_v(node));
        printf(":%i", node_p->w());
        printf(",%i", _synctex_data_g(node));
        printf(",%i", _synctex_data_d(node));
        SYNCTEX_PRINT_CHARINDEX_NL;
        printf("SELF:%p\n", (void *)node);
        printf("    SIBLING:%p\n", (void *)__synctex_tree_sibling(node));
        printf("    PARENT:%p\n", (void *)_synctex_tree_parent(node));
        printf("    LEFT:%p\n", (void *)_synctex_tree_friend(node));
    }
}

static void _synctex_log_void_box(synctex_node_p node)
{
    if (node) {
        printf("%s", synctex_node_isa(node));
        printf(":%i", _synctex_data_t(node));
        printf(",%i", _synctex_data_l(node));
        printf(",%i", _synctex_data_c(node));
        printf(":%i", _synctex_data_h(node));
        printf(",%i", _synctex_data_v(node));
        printf(":%i", node_p->w());
        printf(",%i", _synctex_data_g(node));
        printf(",%i", _synctex_data_d(node));
        SYNCTEX_PRINT_CHARINDEX_NL;
        printf("SELF:%p\n", (void *)node);
        printf("    SIBLING:%p\n", (void *)__synctex_tree_sibling(node));
        printf("    PARENT:%p\n", (void *)_synctex_tree_parent(node));
        printf("    CHILD:%p\n", (void *)_synctex_tree_child(node));
        printf("    LEFT:%p\n", (void *)_synctex_tree_friend(node));
    }
}

static void _synctex_log_vbox(synctex_node_p node)
{
    if (node) {
        printf("%s", synctex_node_isa(node));
        printf(":%i", _synctex_data_t(node));
        printf(",%i", _synctex_data_l(node));
        printf(",%i", _synctex_data_c(node));
        printf(":%i", _synctex_data_h(node));
        printf(",%i", _synctex_data_v(node));
        printf(":%i", node_p->w());
        printf(",%i", _synctex_data_g(node));
        printf(",%i", _synctex_data_d(node));
        SYNCTEX_PRINT_CHARINDEX_NL;
        printf("SELF:%p\n", (void *)node);
        printf("    SIBLING:%p\n", (void *)__synctex_tree_sibling(node));
        printf("    PARENT:%p\n", (void *)_synctex_tree_parent(node));
        printf("    CHILD:%p\n", (void *)_synctex_tree_child(node));
        printf("    LEFT:%p\n", (void *)_synctex_tree_friend(node));
        printf("    NEXT_hbox:%p\n", (void *)_synctex_tree_next_hbox(node));
    }
}

static void _synctex_log_hbox(synctex_node_p node)
{
    if (node) {
        printf("%s", synctex_node_isa(node));
        printf(":%i", _synctex_data_t(node));
        printf(",%i~%i*%i", _synctex_data_l(node), _synctex_data_mean_l(node), _synctex_data_weight(node));
        printf(",%i", _synctex_data_c(node));
        printf(":%i", _synctex_data_h(node));
        printf(",%i", _synctex_data_v(node));
        printf(":%i", node_p->w());
        printf(",%i", _synctex_data_g(node));
        printf(",%i", _synctex_data_d(node));
        printf("/%i", _synctex_data_h_V(node));
        printf(",%i", _synctex_data_v_V(node));
        printf(":%i", _synctex_data_w_V(node));
        printf(",%i", _synctex_data_g_V(node));
        printf(",%i", _synctex_data_d_V(node));
        SYNCTEX_PRINT_CHARINDEX_NL;
        printf("SELF:%p\n", (void *)node);
        printf("    SIBLING:%p\n", (void *)__synctex_tree_sibling(node));
        printf("    PARENT:%p\n", (void *)_synctex_tree_parent(node));
        printf("    CHILD:%p\n", (void *)_synctex_tree_child(node));
        printf("    LEFT:%p\n", (void *)_synctex_tree_friend(node));
        printf("    NEXT_hbox:%p\n", (void *)_synctex_tree_next_hbox(node));
    }
}
static void _synctex_log_proxy(synctex_node_p node)
{
    if (node) {
        synctex_node_p N = _synctex_tree_target(node);
        printf("%s", synctex_node_isa(node));
        printf(":%i", _synctex_data_h(node));
        printf(",%i", _synctex_data_v(node));
        SYNCTEX_PRINT_CHARINDEX_NL;
        printf("SELF:%p\n", (void *)node);
        printf("    SIBLING:%p\n", (void *)__synctex_tree_sibling(node));
        printf("    LEFT:%p\n", (void *)_synctex_tree_friend(node));
        printf("    ->%s\n", _synctex_node_abstract(N));
    }
}
static void _synctex_log_handle(synctex_node_p node)
{
    if (node) {
        synctex_node_p N = _synctex_tree_target(node);
        printf("%s", synctex_node_isa(node));
        SYNCTEX_PRINT_CHARINDEX_NL;
        printf("SELF:%p\n", (void *)node);
        printf("    SIBLING:%p\n", (void *)__synctex_tree_sibling(node));
        printf("    ->%s\n", _synctex_node_abstract(N));
    }
}

void synctex_node_display(synctex_node_p node)
{
    if (node) {
        synctex_manager_p scanR = node->class_->scanner;
        if (scanR) {
            if (scanR->display_switcher < 0) {
                SYNCTEX_MSG_SEND(node, display);
            } else if (scanR->display_switcher > 0 && --scanR->display_switcher > 0) {
                SYNCTEX_MSG_SEND(node, display);
            } else if (scanR->display_switcher-- >= 0) {
                printf("%s Next display skipped. Reset display switcher.\n", node->class_->scanner->display_prompt);
            }
        } else {
            SYNCTEX_MSG_SEND(node, display);
        }
    }
}
static char *_synctex_node_abstract(synctex_node_p node)
{
    SYNCTEX_PARAMETER_ASSERT(node || node->class_);
    return (node && node->class_->abstract) ? node->class_->abstract(node) : "none";
}

static SYNCTEX_INLINE void _synctex_display_child(synctex_node_p node)
{
    synctex_node_p N = _synctex_tree_child(node);
    if (N) {
        _synctex_scanner_display_prompt_down(N->class_->scanner);
        synctex_node_display(N);
        _synctex_scanner_display_prompt_up(N->class_->scanner);
    }
}

static SYNCTEX_INLINE void _synctex_display_sibling(synctex_node_p node)
{
    synctex_node_display(__synctex_tree_sibling(node));
}
#define SYNCTEX_ABSTRACT_MAX 128
static char *_synctex_abstract_input(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "Input:%i:%s(%i)" SYNCTEX_PRINT_CHARINDEX_FMT,
                 _synctex_data_t(node),
                 _synctex_data_name(node),
                 _synctex_data_l(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_input(synctex_node_p node)
{
    if (node) {
        printf("Input:%i:%s(%i)" SYNCTEX_PRINT_CHARINDEX_FMT "\n",
               _synctex_data_t(node),
               _synctex_data_name(node),
               _synctex_data_l(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        synctex_node_display(__synctex_tree_sibling(node));
    }
}

static char *_synctex_abstract_sheet(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract, SYNCTEX_ABSTRACT_MAX, "{%i...}" SYNCTEX_PRINT_CHARINDEX_FMT, _synctex_data_page(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_sheet(synctex_node_p node)
{
    if (node) {
        printf("%s{%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n", node->class_->scanner->display_prompt, _synctex_data_page(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        _synctex_display_child(node);
        printf("%s}\n", node->class_->scanner->display_prompt);
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_form(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract, SYNCTEX_ABSTRACT_MAX, "<%i...>" SYNCTEX_PRINT_CHARINDEX_FMT, _synctex_data_t(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        SYNCTEX_PRINT_CHARINDEX;
    }
    return abstract;
}

static void _synctex_display_form(synctex_node_p node)
{
    if (node) {
        printf("%s<%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n", node->class_->scanner->display_prompt, _synctex_data_t(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        _synctex_display_child(node);
        printf("%s>\n", node->class_->scanner->display_prompt);
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_vbox(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "[%i,%i:%i,%i:%i,%i,%i...]" SYNCTEX_PRINT_CHARINDEX_FMT,
                 _synctex_data_t(node),
                 _synctex_data_l(node),
                 _synctex_data_h(node),
                 _synctex_data_v(node),
                 node_p->w(),
                 _synctex_data_g(node),
                 _synctex_data_d(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_vbox(synctex_node_p node)
{
    if (node) {
        printf("%s[%i,%i:%i,%i:%i,%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n",
               node->class_->scanner->display_prompt,
               _synctex_data_t(node),
               _synctex_data_l(node),
               _synctex_data_h(node),
               _synctex_data_v(node),
               node_p->w(),
               _synctex_data_g(node),
               _synctex_data_d(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        _synctex_display_child(node);
        printf("%s]\n%slast:%s\n",
               node->class_->scanner->display_prompt,
               node->class_->scanner->display_prompt,
               _synctex_node_abstract(_synctex_tree_last(node)));
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_hbox(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "(%i,%i~%i*%i:%i,%i:%i,%i,%i...)" SYNCTEX_PRINT_CHARINDEX_FMT,
                 _synctex_data_t(node),
                 _synctex_data_l(node),
                 _synctex_data_mean_l(node),
                 _synctex_data_weight(node),
                 _synctex_data_h(node),
                 _synctex_data_v(node),
                 node_p->w(),
                 _synctex_data_g(node),
                 _synctex_data_d(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_hbox(synctex_node_p node)
{
    if (node) {
        printf("%s(%i,%i~%i*%i:%i,%i:%i,%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n",
               node->class_->scanner->display_prompt,
               _synctex_data_t(node),
               _synctex_data_l(node),
               _synctex_data_mean_l(node),
               _synctex_data_weight(node),
               _synctex_data_h(node),
               _synctex_data_v(node),
               node_p->w(),
               _synctex_data_g(node),
               _synctex_data_d(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        _synctex_display_child(node);
        printf("%s)\n%slast:%s\n",
               node->class_->scanner->display_prompt,
               node->class_->scanner->display_prompt,
               _synctex_node_abstract(_synctex_tree_last(node)));
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_void_vbox(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "v%i,%i;%i,%i:%i,%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n",
                 _synctex_data_t(node),
                 _synctex_data_l(node),
                 _synctex_data_h(node),
                 _synctex_data_v(node),
                 node_p->w(),
                 _synctex_data_g(node),
                 _synctex_data_d(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_void_vbox(synctex_node_p node)
{
    if (node) {
        printf("%sv%i,%i;%i,%i:%i,%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n",
               node->class_->scanner->display_prompt,
               _synctex_data_t(node),
               _synctex_data_l(node),
               _synctex_data_h(node),
               _synctex_data_v(node),
               node_p->w(),
               _synctex_data_g(node),
               _synctex_data_d(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_void_hbox(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "h%i,%i:%i,%i:%i,%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT,
                 _synctex_data_t(node),
                 _synctex_data_l(node),
                 _synctex_data_h(node),
                 _synctex_data_v(node),
                 node_p->w(),
                 _synctex_data_g(node),
                 _synctex_data_d(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_void_hbox(synctex_node_p node)
{
    if (node) {
        printf("%sh%i,%i:%i,%i:%i,%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n",
               node->class_->scanner->display_prompt,
               _synctex_data_t(node),
               _synctex_data_l(node),
               _synctex_data_h(node),
               _synctex_data_v(node),
               node_p->w(),
               _synctex_data_g(node),
               _synctex_data_d(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_glue(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "glue:%i,%i:%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT,
                 _synctex_data_t(node),
                 _synctex_data_l(node),
                 _synctex_data_h(node),
                 _synctex_data_v(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_glue(synctex_node_p node)
{
    if (node) {
        printf("%sglue:%i,%i:%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n",
               node->class_->scanner->display_prompt,
               _synctex_data_t(node),
               _synctex_data_l(node),
               _synctex_data_h(node),
               _synctex_data_v(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_rule(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "rule:%i,%i:%i,%i:%i,%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT,
                 _synctex_data_t(node),
                 _synctex_data_l(node),
                 _synctex_data_h(node),
                 _synctex_data_v(node),
                 node_p->w(),
                 _synctex_data_g(node),
                 _synctex_data_d(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_rule(synctex_node_p node)
{
    if (node) {
        printf("%srule:%i,%i:%i,%i:%i,%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n",
               node->class_->scanner->display_prompt,
               _synctex_data_t(node),
               _synctex_data_l(node),
               _synctex_data_h(node),
               _synctex_data_v(node),
               node_p->w(),
               _synctex_data_g(node),
               _synctex_data_d(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_math(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "math:%i,%i:%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT,
                 _synctex_data_t(node),
                 _synctex_data_l(node),
                 _synctex_data_h(node),
                 _synctex_data_v(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_math(synctex_node_p node)
{
    if (node) {
        printf("%smath:%i,%i:%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n",
               node->class_->scanner->display_prompt,
               _synctex_data_t(node),
               _synctex_data_l(node),
               _synctex_data_h(node),
               _synctex_data_v(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_kern(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "kern:%i,%i:%i,%i:%i" SYNCTEX_PRINT_CHARINDEX_FMT,
                 _synctex_data_t(node),
                 _synctex_data_l(node),
                 _synctex_data_h(node),
                 _synctex_data_v(node),
                 node_p->w() SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_kern(synctex_node_p node)
{
    if (node) {
        printf("%skern:%i,%i:%i,%i:%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n",
               node->class_->scanner->display_prompt,
               _synctex_data_t(node),
               _synctex_data_l(node),
               _synctex_data_h(node),
               _synctex_data_v(node),
               node_p->w() SYNCTEX_PRINT_CHARINDEX_WHAT);
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_boundary(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "boundary:%i,%i:%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT,
                 _synctex_data_t(node),
                 _synctex_data_l(node),
                 _synctex_data_h(node),
                 _synctex_data_v(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_boundary(synctex_node_p node)
{
    if (node) {
        printf("%sboundary:%i,%i:%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n",
               node->class_->scanner->display_prompt,
               _synctex_data_t(node),
               _synctex_data_l(node),
               _synctex_data_h(node),
               _synctex_data_v(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_box_bdry(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "box bdry:%i,%i:%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT,
                 _synctex_data_t(node),
                 _synctex_data_l(node),
                 _synctex_data_h(node),
                 _synctex_data_v(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_box_bdry(synctex_node_p node)
{
    if (node) {
        printf("%sbox bdry:%i,%i:%i,%i",
               node->class_->scanner->display_prompt,
               _synctex_data_t(node),
               _synctex_data_l(node),
               _synctex_data_h(node),
               _synctex_data_v(node));
        SYNCTEX_PRINT_CHARINDEX_NL;
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_ref(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "form ref:%i:%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT,
                 _synctex_data_t(node),
                 _synctex_data_h(node),
                 _synctex_data_v(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_ref(synctex_node_p node)
{
    if (node) {
        printf("%sform ref:%i:%i,%i", node->class_->scanner->display_prompt, _synctex_data_t(node), _synctex_data_h(node), _synctex_data_v(node));
        SYNCTEX_PRINT_CHARINDEX_NL;
        _synctex_display_sibling(node);
    }
}
static char *_synctex_abstract_proxy(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        synctex_node_p N = _synctex_tree_target(node);
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "%s:%i,%i:%i,%i/%p%s",
                 synctex_node_isa(node),
                 synctex_node_t(node),
                 synctex_node_l(node),
                 _synctex_data_h(node),
                 _synctex_data_v(node),
                 (void *)node, // Fix GCC warning: %p expects a void* according to POSIX
                 _synctex_node_abstract(N));
    }
    return abstract;
}
static void _synctex_display_proxy(synctex_node_p node)
{
    if (node) {
        synctex_node_p N = _synctex_tree_target(node);
        printf("%s%s:%i,%i:%i,%i",
               node->class_->scanner->display_prompt,
               synctex_node_isa(node),
               synctex_node_t(node),
               synctex_node_l(node),
               _synctex_data_h(node),
               _synctex_data_v(node));
        if (N) {
            printf("=%i,%i:%i,%i,%i->%s",
                   synctex_node_h(node),
                   synctex_node_v(node),
                   synctex_node_w(node),
                   synctex_node_g(node),
                   synctex_node_d(node),
                   _synctex_node_abstract(N));
        }
        printf("\n");
        _synctex_display_child(node);
        _synctex_display_sibling(node);
    }
}
static char *_synctex_abstract_proxy_vbox(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "[*%i,%i:%i,%i:%i,%i,%i...*]" SYNCTEX_PRINT_CHARINDEX_FMT,
                 synctex_node_t(node),
                 synctex_node_l(node),
                 synctex_node_h(node),
                 synctex_node_v(node),
                 synctex_node_w(node),
                 synctex_node_g(node),
                 synctex_node_d(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_proxy_vbox(synctex_node_p node)
{
    if (node) {
        printf("%s[*%i,%i:%i,%i:%i,%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n",
               node->class_->scanner->display_prompt,
               synctex_node_t(node),
               synctex_node_l(node),
               synctex_node_h(node),
               synctex_node_v(node),
               synctex_node_w(node),
               synctex_node_g(node),
               synctex_node_d(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        _synctex_display_child(node);
        printf("%s*]\n%slast:%s\n",
               node->class_->scanner->display_prompt,
               node->class_->scanner->display_prompt,
               _synctex_node_abstract(_synctex_tree_last(node)));
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_proxy_hbox(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        snprintf(abstract,
                 SYNCTEX_ABSTRACT_MAX,
                 "(*%i,%i~%i*%i:%i,%i:%i,%i,%i...*)/%p" SYNCTEX_PRINT_CHARINDEX_FMT,
                 synctex_node_t(node),
                 synctex_node_l(node),
                 node->mean_l(),
                 synctex_node_weight(node),
                 synctex_node_h(node),
                 synctex_node_v(node),
                 synctex_node_w(node),
                 synctex_node_g(node),
                 synctex_node_d(node),
                 (void *)node // Fix GCC warning: %p expects a void* according to POSIX
                     SYNCTEX_PRINT_CHARINDEX_WHAT);
    }
    return abstract;
}

static void _synctex_display_proxy_hbox(synctex_node_p node)
{
    if (node) {
        printf("%s(*%i,%i~%i*%i:%i,%i:%i,%i,%i" SYNCTEX_PRINT_CHARINDEX_FMT "\n",
               node->class_->scanner->display_prompt,
               synctex_node_t(node),
               synctex_node_l(node),
               node->mean_l(),
               synctex_node_weight(node),
               synctex_node_h(node),
               synctex_node_v(node),
               synctex_node_w(node),
               synctex_node_g(node),
               synctex_node_d(node) SYNCTEX_PRINT_CHARINDEX_WHAT);
        _synctex_display_child(node);
        printf("%s*)\n%slast:%s\n",
               node->class_->scanner->display_prompt,
               node->class_->scanner->display_prompt,
               _synctex_node_abstract(_synctex_tree_last(node)));
        _synctex_display_sibling(node);
    }
}

static char *_synctex_abstract_handle(synctex_node_p node)
{
    static char abstract[SYNCTEX_ABSTRACT_MAX] = "none";
    if (node) {
        synctex_node_p N = _synctex_tree_target(node);
        if (N && !N->class_) {
            exit(1);
        }
        snprintf(abstract, SYNCTEX_ABSTRACT_MAX, "%s:%s", synctex_node_isa(node), (N ? _synctex_node_abstract(N) : ""));
    }
    return abstract;
}
static void _synctex_display_handle(synctex_node_p node)
{
    if (node) {
        synctex_node_p N = _synctex_tree_target(node);
        printf("%s%s(%i):->%s\n", node->class_->scanner->display_prompt, synctex_node_isa(node), _synctex_data_weight(N), _synctex_node_abstract(N));
        _synctex_display_child(node);
        _synctex_display_sibling(node);
    }
}

static nr_s __synctex_make_input(synctex_manager_p scanner);
/**
 * @brief float+status structure
 *
 * Used to return multiple values from a function.
 */
typedef struct {
    /** float component */
    float value;
    /** error status component */
    Status status;
} _synctex_fs_s;
static _synctex_fs_s _synctex_scan_float_and_dimension(synctex_manager_p scanner);
static Status _synctex_scan_post_scriptum(synctex_manager_p scanner);
static Status _synctex_scan_postamble(synctex_manager_p scanner);
static Status _synctex_setup_visible_hbox(synctex_node_p box);
static Status _synctex_scan_content(synctex_manager_p scanner);
int _synctex_manager_pre_x_offset(synctex_manager_p scanner);
int _synctex_manager_pre_y_offset(synctex_manager_p scanner);


static SYNCTEX_INLINE synctex_node_p __synctex_node_make_friend_tlc(synctex_node_p node);


/**
 *  Free node and its siblings and return its detached child.
 */
static SYNCTEX_INLINE synctex_node_p _synctex_handle_pop_child(synctex_node_p handle)
{
    synctex_node_p child = _synctex_tree_reset_child(handle);
    _synctex_node_free(handle);
    return child;
}
/**
 *  Set the tlc of all the x nodes that are targets of
 *  x_handle and its sibling.
 *  Reset the target of x_handle and deletes its siblings.
 *  child is a node that has just been parsed and is not a boundary node.
 */
static SYNCTEX_INLINE void _synctex_handle_set_tlc(synctex_node_p x_handle, synctex_node_p child, bool make_friend)
{
    if (x_handle) {
        synctex_node_p sibling = x_handle;
        if (child) {
            synctex_node_p target;
            while ((target = synctex_node_target(sibling))) {
                _synctex_data_set_tlc(target, child);
                if (make_friend) {
                    _synctex_node_make_friend_tlc(target);
                }
                if ((sibling = __synctex_tree_sibling(sibling))) {
                    continue;
                } else {
                    break;
                }
            }
        }
        _synctex_tree_reset_target(x_handle);
        sibling = __synctex_tree_reset_sibling(x_handle);
        _synctex_node_free(sibling);
    }
}
/**
 *  When we have parsed a box, we must register
 *  all the contained heading boundary nodes
 *  that have not yet been registered.
 *  Those handles will be deleted when poping.
 */
static SYNCTEX_INLINE void _synctex_handle_make_friend_tlc(synctex_node_p node)
{
    while (node) {
        synctex_node_p target = _synctex_tree_reset_target(node);
        _synctex_node_make_friend_tlc(target);
        node = __synctex_tree_sibling(node);
    }
}

 #undef SYNCTEX_RETURN


synctex_node_p synctex_scanner_input(synctex_manager_p scanner)
{
    return scanner ? scanner->input : nullptr;
}
synctex_node_p synctex_scanner_input_with_t(synctex_manager_p scanner, int t) {
    synctex_node_p input = scanner?scanner->input:nullptr;
    while (input && _synctex_data_t(input)!=tag) {
        input = __synctex_tree_sibling(input);
    }
    return input;
}
int synctex_scanner_get_data_version(synctex_manager_p scanner)
{
    return nullptr != scanner ? scanner->version : 0;
}
const char *synctex_scanner_get_output_fmt(synctex_manager_p scanner)
{
    return nullptr != scanner && scanner->output_fmt ? scanner->output_fmt : "";
}
const char *synctex_scanner_get_output(synctex_manager_p scanner)
{
    return nullptr != scanner && scanner->reader->output ? scanner->reader->output : "";
}
const char *synctex_scanner_get_synctex(synctex_manager_p scanner)
{
    return nullptr != scanner && scanner->reader->synctex ? scanner->reader->synctex : "";
}

#define SYNCTEX_DEFINE_NODE_HVWHD(WHAT)                                                                                                                        \
    int synctex_node_##WHAT(synctex_node_p node)                                                                                                               \
    {                                                                                                                                                          \
        return (node && node->class_->inspector->WHAT) ? node->class_->inspector->WHAT(node) : 0;                                                              \
    }
#define SYNCTEX_DEFINE_PROXY_HV(WHAT)                                                                                                                          \
    static int _synctex_proxy_##WHAT(_synctex_proxy_p proxy)                                                                                                   \
    {                                                                                                                                                          \
        synctex_node_p target = _synctex_tree_target(proxy);                                                                                                   \
        if (target) {                                                                                                                                          \
            return _synctex_data_##WHAT(proxy) + synctex_node_##WHAT(target);                                                                                  \
        } else {                                                                                                                                               \
            return proxy ? _synctex_data_##WHAT(proxy) : 0;                                                                                                    \
        }                                                                                                                                                      \
    }
#define SYNCTEX_DEFINE_PROXY_TLCWVD(WHAT)                                                                                                                      \
    static int _synctex_proxy_##WHAT(_synctex_proxy_p proxy)                                                                                                   \
    {                                                                                                                                                          \
        synctex_node_p target = _synctex_tree_target(proxy);                                                                                                   \
        return target ? synctex_node_##WHAT(target) : 0;                                                                                                       \
    }

/**
 *  The horizontal location of the node.
 *  Idem for v, width, height and depth.
 *  - parameter node: a node with geometrical information.
 *  - returns: an integer.
 *  - requires: every proxy node has a target.
 *  - note: recursive call if the parameter has a proxy.
 *  - author: JL
 */
SYNCTEX_DEFINE_NODE_HVWHD(h);
SYNCTEX_DEFINE_NODE_HVWHD(v);
SYNCTEX_DEFINE_NODE_HVWHD(width);
SYNCTEX_DEFINE_NODE_HVWHD(height);
SYNCTEX_DEFINE_NODE_HVWHD(depth);
SYNCTEX_DEFINE_PROXY_TLCWVD(tag);
SYNCTEX_DEFINE_PROXY_TLCWVD(line);
SYNCTEX_DEFINE_PROXY_TLCWVD(column);
SYNCTEX_DEFINE_PROXY_HV(h);
SYNCTEX_DEFINE_PROXY_HV(v);
SYNCTEX_DEFINE_PROXY_TLCWVD(width);
SYNCTEX_DEFINE_PROXY_TLCWVD(height);
SYNCTEX_DEFINE_PROXY_TLCWVD(depth);

/**
 *  Whether the argument is a box,
 *  either vertical or horizontal,
 *  either void or not,
 *  or a proxy to such a box.
 *  - parameter NODE: of type synctex_node_p
 *  - returns: yorn
 */

static SYNCTEX_INLINE bool _synctex_node_is_box(synctex_node_p node)
{
    return node
        && (node->class_->type == synctex_node_type_hbox || node->class_->type == synctex_node_type_void_hbox || node->class_->type == synctex_node_type_vbox
            || node->class_->type == synctex_node_type_void_vbox || _synctex_node_is_box(_synctex_tree_target(node)));
}

/**
 *  Whether the argument is a handle.
 *  Handles are similar to proxies because they have a target.
 *  They are used for query results.
 *  - parameter NODE: of type synctex_node_p
 *  - returns: yorn
 */

static SYNCTEX_INLINE bool _synctex_node_is_handle(synctex_node_p node)
{
    return node && (node->class_->type == synctex_node_type_handle);
}

/**
 *  Resolves handle indirection.
 *  - parameter node: of type synctex_node_p
 *  - returns: node if it is not a handle,
 *  its target otherwise.
 */

static SYNCTEX_INLINE synctex_node_p _synctex_node_or_handle_target(synctex_node_p node)
{
    return _synctex_node_is_handle(node) ? _synctex_tree_target(node) : node;
}

/**
 *  Whether the argument is an hbox.
 *  - parameter NODE: of type synctex_node_p
 *  - returns: yorn
 */

static SYNCTEX_INLINE bool _synctex_node_is_hbox(synctex_node_p node)
{
    return node
        && (node->class_->type == synctex_node_type_hbox || node->class_->type == synctex_node_type_void_hbox
            || _synctex_node_is_hbox(_synctex_tree_target(node)));
}

/**
 * @brief The horizontal location of the first box enclosing node.
 *
 * The first box enclosing a box is itself.
 * @param node a node with geometrical information.
 * @return int in TeX sp coordinates.
 */
int synctex_node_box_h(synctex_node_p node)
{
    if (_synctex_node_is_box(node) || (node = _synctex_tree_parent(node))) {
        return synctex_node_h(node);
    }
    return 0;
}
/**
 * @brief The vertical location of the first box enclosing node.
 *
 * The first box enclosing a box is itself.
 * @param node a node with geometrical information.
 * @return int in TeX sp coordinates
 */
int synctex_node_box_v(synctex_node_p node)
{
    if (_synctex_node_is_box(node) || (node = _synctex_tree_parent(node))) {
        return synctex_node_v(node);
    }
    return 0;
}
/**
 *  The width of the first box enclosing node.
 *  - parameter node: a node with geometrical information.
 *  - returns: an integer.
 *  - author: JL
 */
int synctex_node_box_w(synctex_node_p node)
{
    if (_synctex_node_is_box(node) || (node = _synctex_tree_parent(node))) {
        return synctex_node_w(node);
    }
    return 0;
}
/**
 *  The height of the first box enclosing node.
 *  - parameter node: a node with geometrical information.
 *  - returns: an integer.
 *  - author: JL
 */
int synctex_node_box_g(synctex_node_p node)
{
    if (_synctex_node_is_box(node) || (node = _synctex_tree_parent(node))) {
        return synctex_node_g(node);
    }
    return 0;
}
/**
 *  The depth of the first box enclosing node.
 *  - parameter node: a node with geometrical information.
 *  - returns: an integer.
 *  - author: JL
 */
int synctex_node_box_d(synctex_node_p node)
{
    if (_synctex_node_is_box(node) || (node = _synctex_tree_parent(node))) {
        return synctex_node_d(node);
    }
    return 0;
}
/**
 *  The horizontal location of an hbox, corrected with contents.
 *  - parameter node: an hbox node.
 *  - returns: an integer, 0 if node is not an hbox or an hbox proxy.
 *  - note: recursive call when node is an hbox proxy.
 *  - author: JL
 */
int synctex_node_hbox_h(synctex_node_p node)
{
    switch (synctex_node_type(node)) {
    case synctex_node_type_hbox:
        return _synctex_data_h_V(node);
    case synctex_node_type_proxy_hbox:
        return _synctex_data_h(node) + synctex_node_hbox_h(_synctex_tree_target(node));
    default:
        return 0;
    }
}
/**
 *  The vertical location of an hbox, corrected with contents.
 *  - parameter node: an hbox node.
 *  - returns: an integer, 0 if node is not an hbox or an hbox proxy.
 *  - note: recursive call when node is an hbox proxy.
 *  - author: JL
 */
int synctex_node_hbox_v(synctex_node_p node)
{
    switch (synctex_node_type(node)) {
    case synctex_node_type_hbox:
        return _synctex_data_v_V(node);
    case synctex_node_type_proxy_hbox:
        return _synctex_data_v(node) + synctex_node_hbox_v(_synctex_tree_target(node));
    default:
        return 0;
    }
}
/**
 *  The width of an hbox, corrected with contents.
 *  - parameter node: an hbox node, 0 if node is not an hbox or an hbox proxy.
 *  - returns: an integer.
 *  - author: JL
 */
int synctex_node_hbox_w(synctex_node_p node)
{
    synctex_node_p target = _synctex_tree_target(node);
    if (target) {
        node = target;
    }
    return synctex_node_type(node) == synctex_node_type_hbox ? _synctex_data_w_V(node) : 0;
}
/**
 *  The height of an hbox, corrected with contents.
 *  - parameter node: an hbox node.
 *  - returns: an integer, 0 if node is not an hbox or an hbox proxy.
 *  - author: JL
 */
int synctex_node_hbox_g(synctex_node_p node)
{
    synctex_node_p target = _synctex_tree_target(node);
    if (target) {
        node = target;
    }
    return synctex_node_type(node) == synctex_node_type_hbox ? _synctex_data_g_V(node) : 0;
}
/**
 *  The depth of an hbox, corrected with contents.
 *  - parameter node: an hbox node.
 *  - returns: an integer, 0 if node is not an hbox or an hbox proxy.
 *  - note: recursive call when node is an hbox proxy.
 *  - author: JL
 */
int synctex_node_hbox_d(synctex_node_p node)
{
    synctex_node_p target = _synctex_tree_target(node);
    if (target) {
        node = target;
    }
    return synctex_node_type(node) == synctex_node_type_hbox ? _synctex_data_d_V(node) : 0;
}

#define SYNCTEX_VISIBLE_SIZE(node, s) (s) * node->class_->scanner->unit
#define SYNCTEX_VISIBLE_DISTANCE_h(node, d) ((d) * node->class_->scanner->unit + node->class_->scanner->x_offset)
#define SYNCTEX_VISIBLE_DISTANCE_v(node, d) ((d) * node->class_->scanner->unit + node->class_->scanner->y_offset)
static float __synctex_node_visible_h(synctex_node_p node)
{
    return SYNCTEX_VISIBLE_DISTANCE_h(node, synctex_node_h(node));
}
static float __synctex_node_visible_v(synctex_node_p node)
{
    return SYNCTEX_VISIBLE_DISTANCE_v(node, synctex_node_v(node));
}
static float __synctex_node_visible_w(synctex_node_p node)
{
    return SYNCTEX_VISIBLE_SIZE(node, synctex_node_w(node));
}
static float __synctex_node_visible_g(synctex_node_p node)
{
    return SYNCTEX_VISIBLE_SIZE(node, synctex_node_g(node));
}
static float __synctex_node_visible_d(synctex_node_p node)
{
    return SYNCTEX_VISIBLE_SIZE(node, synctex_node_d(node));
}
static float __synctex_proxy_visible_h(synctex_node_p node)
{
    return SYNCTEX_VISIBLE_DISTANCE_h(node, synctex_node_h(node));
}
static float __synctex_proxy_visible_v(synctex_node_p node)
{
    return SYNCTEX_VISIBLE_DISTANCE_v(node, synctex_node_v(node));
}
static float __synctex_proxy_visible_w(synctex_node_p node)
{
    synctex_node_p target = _synctex_tree_target(node);
    return __synctex_node_visible_w(target);
}
static float __synctex_proxy_visible_g(synctex_node_p node)
{
    synctex_node_p target = _synctex_tree_target(node);
    return __synctex_node_visible_g(target);
}
static float __synctex_proxy_visible_d(synctex_node_p node)
{
    synctex_node_p target = _synctex_tree_target(node);
    return __synctex_node_visible_d(target);
}
static float __synctex_kern_visible_h(_synctex_noxy_p noxy)
{
    int h = _synctex_data_h(noxy);
    int w = noxy_p->w();
    return SYNCTEX_VISIBLE_DISTANCE_h(noxy, width > 0 ? h - width : h);
}
static float __synctex_kern_visible_w(_synctex_noxy_p noxy)
{
    int w = noxy_p->w();
    return SYNCTEX_VISIBLE_SIZE(noxy, width > 0 ? width : -width);
}
static float __synctex_rule_visible_h(_synctex_noxy_p noxy)
{
    int h = _synctex_data_h(noxy);
    int w = noxy_p->w();
    return SYNCTEX_VISIBLE_DISTANCE_h(noxy, width > 0 ? h : h - width);
}
static float __synctex_rule_visible_w(_synctex_noxy_p noxy)
{
    int w = noxy_p->w();
    return SYNCTEX_VISIBLE_SIZE(noxy, width > 0 ? width : -width);
}
static float __synctex_rule_visible_v(_synctex_noxy_p noxy)
{
    return __synctex_node_visible_v(noxy);
}
static float __synctex_rule_visible_g(_synctex_noxy_p noxy)
{
    return __synctex_node_visible_g(noxy);
}
static float __synctex_rule_visible_d(_synctex_noxy_p noxy)
{
    return __synctex_node_visible_d(noxy);
}

/**
 *  The horizontal location of node, in page coordinates.
 *  - parameter node: a node.
 *  - returns: a float.
 *  - author: JL
 */
float synctex_node_visible_h(synctex_node_p node)
{
    return node ? node->class_->vispector->h(node) : 0;
}
/**
 *  The vertical location of node, in page coordinates.
 *  - parameter node: a node.
 *  - returns: a float.
 *  - author: JL
 */
float synctex_node_visible_v(synctex_node_p node)
{
    return node ? node->class_->vispector->v(node) : 0;
}
/**
 *  The width of node, in page coordinates.
 *  - parameter node: a node.
 *  - returns: a float.
 *  - author: JL
 */
float synctex_node_visible_w(synctex_node_p node)
{
    return node ? node->class_->vispector->w(node) : 0;
}
/**
 *  The height of node, in page coordinates.
 *  - parameter node: a node.
 *  - returns: a float.
 *  - author: JL
 */
float synctex_node_visible_g(synctex_node_p node)
{
    return node ? node->class_->vispector->g(node) : 0;
}
/**
 *  The depth of node, in page coordinates.
 *  - parameter node: a node.
 *  - returns: a float.
 *  - author: JL
 */
float synctex_node_visible_d(synctex_node_p node)
{
    return node ? node->class_->vispector->d(node) : 0;
}

/**
 *  The WithV variant of geometrical information.
 *  - parameter node: a node.
 *  - returns: an integer.
 *  - author: JL
 */
#define SYNCTEX_DEFINE_V(WHAT)                                                                                                                                 \
    static SYNCTEX_INLINE int _synctex_node_##WHAT##_V(synctex_node_p node)                                                                                    \
    {                                                                                                                                                          \
        synctex_node_p target = _synctex_tree_target(node);                                                                                                    \
        if (target) {                                                                                                                                          \
            return _synctex_data_##WHAT(node) + _synctex_node_##WHAT##_V(target);                                                                              \
        } else if (_synctex_data_has_##WHAT##_V(node)) {                                                                                                       \
            return _synctex_data_##WHAT##_V(node);                                                                                                             \
        } else {                                                                                                                                               \
            return _synctex_data_##WHAT(node);                                                                                                                 \
        }                                                                                                                                                      \
    }
/*
Definitions of
_synctex_node_h_V
_synctex_node_v_V
_synctex_node_w_V
_synctex_node_g_V
_synctex_node_d_V
*/
SYNCTEX_DEFINE_V(h);
SYNCTEX_DEFINE_V(v);
SYNCTEX_DEFINE_V(width);
SYNCTEX_DEFINE_V(height);
SYNCTEX_DEFINE_V(depth);


/**
 *  The higher box node in the parent hierarchy which
 *  mean line number is the one of node ±1.
 *  This enclosing box is computed as follows
 *  1) get the first hbox in the parent linked list
 *  starting at node.
 *  If there is none, simply return the parent of node.
 *  2) compute the mean line number
 *  3) scans up the tree for the higher hbox with
 *  the same mean line number, ±1 eventually
 *  - parameter node: a node.
 *  - returns: a (proxy to a) box node.
 *  - author: JL
 */
static synctex_node_p _synctex_node_box_visible(synctex_node_p node)
{
    if ((node = _synctex_node_or_handle_target(node))) {
        int mean = 0;
        int bound = 1500000 / (node->class_->scanner->pre_magnification / 1000.0);
        synctex_node_p parent = nullptr;
        /*  get the first enclosing parent
         *  then get the highest enclosing parent with the same mean line ±1 */
        node = _synctex_node_or_handle_target(node);
        if (!_synctex_node_is_box(node)) {
            if ((parent = _synctex_tree_parent(node))) {
                node = parent;
            } else if ((node = _synctex_tree_target(node))) {
                if (!_synctex_node_is_box(node)) {
                    if ((parent = _synctex_tree_parent(node))) {
                        node = parent;
                    } else {
                        return nullptr;
                    }
                }
            }
        }
        parent = node;
        mean = node->mean_l();
        while ((parent = _synctex_tree_parent(parent))) {
            if (_synctex_node_is_hbox(parent)) {
                if (std::abs(mean - parent->mean_l()) > 1) {
                    return node;
                } else if (synctex_node_w(parent) > bound) {
                    return parent;
                } else if (synctex_node_g(parent) + synctex_node_d(parent) > bound) {
                    return parent;
                }
                node = parent;
            }
        }
    }
    return node;
}
/**
 *  The horizontal location of the first box enclosing node, in page coordinates.
 *  - parameter node: a node.
 *  - returns: a float.
 *  - author: JL
 */
float synctex_node_box_visible_h(synctex_node_p node)
{
    return SYNCTEX_VISIBLE_DISTANCE_h(node, _synctex_node_h_V(_synctex_node_box_visible(node)));
}
/**
 *  The vertical location of the first box enclosing node, in page coordinates.
 *  - parameter node: a node.
 *  - returns: a float.
 *  - author: JL
 */
float synctex_node_box_visible_v(synctex_node_p node)
{
    return SYNCTEX_VISIBLE_DISTANCE_v(node, _synctex_node_v_V(_synctex_node_box_visible(node)));
}
/**
 *  The width of the first box enclosing node, in page coordinates.
 *  - parameter node: a node.
 *  - returns: a float.
 *  - author: JL
 */
float synctex_node_box_visible_w(synctex_node_p node)
{
    return SYNCTEX_VISIBLE_SIZE(node, _synctex_node_w_V(_synctex_node_box_visible(node)));
}
/**
 *  The height of the first box enclosing node, in page coordinates.
 *  - parameter node: a node.
 *  - returns: a float.
 *  - author: JL
 */
float synctex_node_box_visible_g(synctex_node_p node)
{
    return SYNCTEX_VISIBLE_SIZE(node, _synctex_node_g_V(_synctex_node_box_visible(node)));
}
/**
 *  The depth of the first box enclosing node, in page coordinates.
 *  - parameter node: a node.
 *  - returns: a float.
 *  - author: JL
 */
float synctex_node_box_visible_d(synctex_node_p node)
{
    return SYNCTEX_VISIBLE_SIZE(node, _synctex_node_d_V(_synctex_node_box_visible(node)));
}

/**
 *  The page number of the sheet enclosing node.
 *  - parameter node: a node.
 *  - returns: the page number or -1 if node does not belong to a sheet tree.
 *  - note: a proxy target does not belong to a sheet
 *      but a form, its page number is always -1.
 *  - note: a handles does not belong to a sheet not a form.
 *      its page number is -1.
 *  - author: JL
 */
int synctex_node_page(synctex_node_p node)
{
    synctex_node_p parent = nullptr;
    while ((parent = _synctex_tree_parent(node))) {
        node = parent;
    }
    if (synctex_node_type(node) == synctex_node_type_sheet) {
        return _synctex_data_page(node);
    }
    return -1;
}
/**
 *  The page number of the target.
 *  - author: JL
 */
static SYNCTEX_INLINE int _synctex_node_target_page(synctex_node_p node)
{
    return synctex_node_page(_synctex_tree_target(node));
}

#if defined(SYNCTEX_USE_CHARINDEX)
synctex_charindex_t synctex_node_charindex(synctex_node_p node)
{
    synctex_node_p target = _synctex_tree_target(node);
    return target ? SYNCTEX_CHARINDEX(target) : (node ? SYNCTEX_CHARINDEX(node) : 0);
}

/**
 *  The tag of the node.
 *  - parameter node: a node.
 *  - returns: the tag or -1 if node is nullptr.
 *  - author: JL
 */
int synctex_node_t(synctex_node_p node)
{
    return node ? node->class_->tlcpector->LR::t(node) : -1;
}
/**
 *  The line of the node.
 *  - parameter node: a node.
 *  - returns: the line or -1 if node is nullptr.
 *  - author: JL
 */
int synctex_node_l(synctex_node_p node)
{
    return node ? node->class_->tlcpector->LR::l(node) : -1;
}
/**
 *  The column of the node.
 *  - parameter node: a node.
 *  - returns: the column or -1 if node is nullptr.
 *  - author: JL
 */
int synctex_node_c(synctex_node_p node)
{
    return node ? node->class_->tlcpector->LR::c(node) : -1;
}
/**
 *  The mean line number of the node.
 *  - parameter node: a node.
 *  - returns: the mean line or -1 if node is nullptr.
 *  - author: JL
 */
int synctex_node_p node->mean_l()
{
    synctex_node_p other = _synctex_tree_target(node);
    if (other) {
        node = other;
    }
    if (_synctex_data_has_mean_l(node)) {
        return _synctex_data_mean_l(node);
    }
    if ((other = synctex_node_parent(node))) {
        if (_synctex_data_has_mean_l(other)) {
            return _synctex_data_mean_l(other);
        }
    }
    return synctex_node_l(node);
}

/**
 * @brief The weight of the node.
 *
 * @param node
 * @return int. -1 if node is nullptr, 0 if the node is not an hbox.
 * @author Jérôme LAURENS
 */
int synctex_node_weight(synctex_node_p node)
{
    synctex_node_p target = _synctex_tree_target(node);
    if (target) {
        node = target;
    }
    return node ? (synctex_node_type(node) == synctex_node_type_hbox ? _synctex_data_weight(node) : 0) : -1;
}
/**
 *  The number of children of the node.
 *  - parameter node: a node.
 *  - returns: the count or -1 if node is nullptr.
 *  - author: JL
 */
int synctex_node_child_count(synctex_node_p node)
{
    synctex_node_p target = _synctex_tree_target(node);
    if (target) {
        node = target;
    }
    return node ? (synctex_node_type(node) == synctex_node_type_hbox ? _synctex_data_weight(node) : 0) : -1;
}

/**
 *  The sheet of the scanner with a given page number.
 *  - parameter scanner: a scanner.
 *  - parameter page: a 1 based page number.
 *      If page == 0, returns the first sheet.
 *  - returns: a sheet or nullptr.
 *  - author: JL
 */
synctex_node_p synctex_sheet(synctex_manager_p scanner, int page)
{
    if (scanner) {
        synctex_node_p sheet = scanner->sheet;
        while (sheet) {
            if (page == _synctex_data_page(sheet)) {
                return sheet;
            }
            sheet = __synctex_tree_sibling(sheet);
        }
        if (page == 0) {
            return scanner->sheet;
        }
    }
    return nullptr;
}
/**
 *  The form of the scanner with a given tag.
 *  - parameter scanner: a scanner.
 *  - parameter tag: an integer identifier.
 *      If tag == 0, returns the first form.
 *  - returns: a form.
 *  - author: JL
 */
synctex_node_p synctex_form(synctex_manager_p scanner, int t)
{
    if (scanner) {
        synctex_node_p form = scanner->form;
        while (form) {
            if (tag == _synctex_data_t(form)) {
                return form;
            }
            form = __synctex_tree_sibling(form);
        }
        if (tag == 0) {
            return scanner->form;
        }
    }
    return nullptr;
}

/**
 *  The content of the sheet with given page number.
 *  - parameter scanner: a scanner.
 *  - parameter page: a 1 based page number.
 *  - returns: a (vertical) box node.
 *  - author: JL
 */
synctex_node_p synctex_sheet_content(synctex_manager_p scanner, int page)
{
    if (scanner) {
        return _synctex_tree_child(synctex_sheet(scanner, page));
    }
    return nullptr;
}


static SYNCTEX_INLINE synctex_node_p _synctex_scanner_friend(synctex_manager_p scanner, int i)
{
    if (i >= 0) {
        i = std::abs(i) % (scanner->number_of_lists);
        return (scanner->lists_of_friends)[i];
    }
    return nullptr;
}
static SYNCTEX_INLINE bool _synctex_nodes_are_friend(synctex_node_p left, synctex_node_p right)
{
    return synctex_node_t(left) == synctex_node_t(right) && synctex_node_l(left) == synctex_node_l(right);
}
/**
 *  The sibling argument is a parent/child list of nodes of the same page.
 */
typedef struct {
    int count;
    synctex_node_p node;
} _synctex_counted_node_s;

static SYNCTEX_INLINE _synctex_counted_node_s _synctex_vertically_sorted_v2(synctex_node_p sibling)
{
    /* Clean the weights of the parents */
    _synctex_counted_node_s result = {0, nullptr};
    synctex_node_p h = nullptr;
    synctex_node_p next_h = nullptr;
    synctex_node_p parent = nullptr;
    int weight = 0;
    synctex_node_p N = nullptr;
    h = sibling;
    do {
        N = _synctex_tree_target(h);
        parent = _synctex_tree_parent(N);
        _synctex_data_set_weight(parent, 0);
    } while ((h = _synctex_tree_child(h)));
    /* Compute the weights of the nodes */
    h = sibling;
    do {
        N = _synctex_tree_target(h);
        parent = _synctex_tree_parent(N);
        weight = _synctex_data_weight(parent);
        if (weight == 0) {
            N = _synctex_tree_child(parent);
            do {
                if (_synctex_nodes_are_friend(N, sibling)) {
                    ++weight;
                }
            } while ((N = __synctex_tree_sibling(N)));
            _synctex_data_set_weight(h, weight);
            _synctex_data_set_weight(parent, weight);
        }
    } while ((h = _synctex_tree_child(h)));
    /* Order handle nodes according to the weight */
    h = _synctex_tree_reset_child(sibling);
    result.node = sibling;
    weight = 0;
    while ((h)) {
        N = result.node;
        if (_synctex_data_weight(h) > _synctex_data_weight(N)) {
            next_h = _synctex_tree_set_child(h, N);
            result.node = h;
        } else if (_synctex_data_weight(h) == 0) {
            ++weight;
            next_h = _synctex_tree_reset_child(h);
            _synctex_node_free(h);
        } else {
            synctex_node_p next_N = nullptr;
            while ((next_N = _synctex_tree_child(N))) {
                N = next_N;
                if (_synctex_data_weight(h) < _synctex_data_weight(next_N)) {
                    continue;
                }
                break;
            }
            next_h = _synctex_tree_set_child(h, _synctex_tree_set_child(N, h));
        }
        h = next_h;
    };
    h = result.node;
    weight = 0;
    do {
        ++weight;
    } while ((h = _synctex_tree_child(h)));
    result.count = 1;
    h = result.node;
    while ((next_h = _synctex_tree_child(h))) {
        if (_synctex_data_weight(next_h) == 0) {
            _synctex_tree_reset_child(h);
            weight = 1;
            h = next_h;
            while ((h = _synctex_tree_child(h))) {
                ++weight;
            }
            _synctex_node_free(next_h);
            break;
        }
        ++result.count;
        h = next_h;
    }
    return result;
}

static SYNCTEX_INLINE bool _synctex_point_in_box_v2(synctex_point_p hitP, synctex_node_p node);

/*  This struct records distances, the left one is non negative and the right one is non positive.
 *  When comparing the locations of 2 different graphical objects on the page, we will have to also record the
 *  horizontal distance as signed to keep track of the typesetting order.*/

struct ni_s {
    Node_p node_p;
    int distance;
};

using Nd_p = ni_s *;

#define SYNCTEX_ND_0  (ni_s) { nullptr, std::numeric_limits<int>::max() }

struct lr_nd_s {
    ni_s l;
    ni_s r;
};

/*  The best container is the deeper box that contains the hit point (WithH,WithV).
 *  _synctex_closest_container_below starts with node whereas
 *  _synctex_box_child_deepest starts with node's children, if any
 *  if node is not a box, or a void box, nullptr is returned.
 *  We traverse the node tree in a deep first manner and stop as soon as a result is found. */
static synctex_node_p _synctex_closest_container_below(synctex_point_p hitP, synctex_node_p node);

static SYNCTEX_INLINE lr_nd_s _synctex_eq_get_closest_children_in_box_v2(synctex_point_p hitP, synctex_node_p node);

/*  Closest child, recursive.  */
static ni_s __synctex_closest_deep_child_v2(synctex_point_p hitP, synctex_node_p node);

/*  The smallest container between two has the smallest width or height.
 *  This comparison is used when there are 2 overlapping boxes that contain the hit point.
 *  For ConTeXt, the problem appears at each page.
 *  The chosen box is the one with the smallest height, then the smallest width. */
static SYNCTEX_INLINE synctex_node_p _synctex_smallest_container_v2(synctex_node_p node, synctex_node_p other_node);

/*  Returns the distance between the hit point hit point=(WithH,WithV) and the given node. */

static int _synctex_point_node_distance_v2(synctex_point_p hitP, synctex_node_p node);

/*  The closest container is the box that is the one closest to the given point.
 *  The "visible" version takes into account the visible dimensions instead of the real ones given by TeX. */
static ni_s _synctex_eq_closest_child_v2(synctex_point_p hitP, synctex_node_p node);

synctex_node_p synctex_node_target(synctex_node_p node)
{
    return _synctex_tree_target(node);
}

static SYNCTEX_INLINE bool _synctex_point_in_box_v2(synctex_point_p hit, synctex_node_p node)
{
    if (node) {
        if (0 == _synctex_h_relative_to(hit, node).i && 0 == _synctex_v_relative_to(hit, node).i) {
            return synctex_YES;
        }
    }
    return synctex_NO;
}

static ni_s _synctex_eq_deepest_container_v3(synctex_point_p hit, synctex_node_p node)
{
    if (node) {
        synctex_node_p child = nullptr;
        if ((child = synctex_node_child(node))) {
            /*  Non void hbox, vbox, box proxy or form ref */
            /*  We go deep first because some boxes have 0 dimensions
             *  despite they do contain some black material.
             */
            do {
                ni_s deep = _synctex_eq_deepest_container_v3(hit, child);
                if (deep.node) {
                    /*  One of the children contains the hit-> */
                    return deep;
                }
            } while ((child = synctex_node_sibling(child)));
            /*  For vboxes we try to use some node inside.
             *  Walk through the list of siblings until we find the closest one.
             *  Only consider siblings with children inside. */
            if (synctex_node_type(node) == synctex_node_type_vbox || synctex_node_type(node) == synctex_node_type_proxy_vbox) {
                if ((child = synctex_node_child(node))) {
                    ni_s best = SYNCTEX_ND_0;
                    do {
                        if (synctex_node_child(child)) {
                            int d = _synctex_point_node_distance_v2(hit, child);
                            if (d < best.i) {
                                best = (ni_s){child, d};
                            }
                        }
                    } while ((child = synctex_node_sibling(child)));
                    if (best.node) {
                        return best;
                    }
                }
            }
            /*  is the hit point inside the box? */
            if (_synctex_point_in_box_v2(hit, node)) {
                return (ni_s){node, 0};
            }
        }
    }
    return SYNCTEX_ND_0;
}

/*  Compares the locations of the hit point with the locations of
 *  the various nodes contained in the box.
 *  As it is an horizontal box, we only compare horizontal coordinates.
 */
static SYNCTEX_INLINE lr_nd_s __synctex_eq_get_closest_children_in_hbox_v2(synctex_point_p hitP, synctex_node_p node)
{
    ni_s childd = SYNCTEX_ND_0;
    lr_nd_s nds = {SYNCTEX_ND_0, SYNCTEX_ND_0};
    if ((childd.node = synctex_node_child(node))) {
        ni_s nd = SYNCTEX_ND_0;
        do {
            childd = _synctex_h_relative_to(hitP, childd.node);
            if (childd.i > 0) {
                /*  node is to the right of the hit point.
                 *  We compare node and the previously recorded one, through the recorded distance.
                 *  If the nodes have the same tag, prefer the one with the smallest line number,
                 *  if the nodes also have the same line number, prefer the one with the smallest column. */
                if (nds.r.i > childd.i) {
                    nds.r = childd;
                } else if (nds.r.i == childd.i && nds.r.node) {
                    if (_synctex_data_t(nds.r.node) == _synctex_data_t(childd.node)
                        && (_synctex_data_l(nds.r.node) > _synctex_data_l(childd.node)
                            || (_synctex_data_l(nds.r.node) == _synctex_data_l(childd.node)
                                && _synctex_data_c(nds.r.node) > _synctex_data_c(childd.node)))) {
                        nds.r = childd;
                    }
                }
            } else if (childd.i == 0) {
                /*  hit point is inside node. */
                if (_synctex_tree_child(childd.node)) {
                    return _synctex_eq_get_closest_children_in_box_v2(hitP, childd.node);
                }
                nds.l = childd;
            } else { /*  here childd.i < 0, the hit point is to the right of node */
                childd.i = -childd.i;
                if (nds.l.i > childd.i) {
                    nds.l = childd;
                } else if (nds.l.i == childd.i && nds.l.node) {
                    if (_synctex_data_t(nds.l.node) == _synctex_data_t(childd.node)
                        && (_synctex_data_l(nds.l.node) > _synctex_data_l(childd.node)
                            || (_synctex_data_l(nds.l.node) == _synctex_data_l(childd.node)
                                && _synctex_data_c(nds.l.node) > _synctex_data_c(childd.node)))) {
                        nds.l = childd;
                    }
                }
            }
        } while ((childd.node = synctex_node_sibling(childd.node)));
        if (nds.l.node) {
            /*  the left node is new, try to narrow the result */
            if ((nd = _synctex_eq_deepest_container_v3(hitP, nds.l.node)).node) {
                nds.l = nd;
            }
            if ((nd = __synctex_closest_deep_child_v2(hitP, nds.l.node)).node) {
                nds.l.node = nd.node;
            }
        }
        if (nds.r.node) {
            /*  the right node is new, try to narrow the result */
            if ((nd = _synctex_eq_deepest_container_v3(hitP, nds.r.node)).node) {
                nds.r = nd;
            }
            if ((nd = __synctex_closest_deep_child_v2(hitP, nds.r.node)).node) {
                nds.r.node = nd.node;
            }
        }
    }
    return nds;
}

#if 0


static void _synctex_node_dump(synctex_node_p node, synctex_printer_f printer, int *depth);

#define SYNCTEX_TMP_ITLHV(node) synctex_node_isa(node), synctex_node_t(node), synctex_node_l(node), synctex_node_h(node), synctex_node_v(node)

#define SYNCTEX_TMP_ITLHVWHD(node) SYNCTEX_TMP_ITLHV(node), synctex_node_w(node), synctex_node_g(node), synctex_node_d(node)

static void _synctex_node_dump(synctex_node_p node, synctex_printer_f printer, int *depth)
{
    synctex_node_p N;
    switch (synctex_node_type(node)) {
    case synctex_node_type_vbox:
        (*printer)("%s%c%s:%i:%i:%i:%i:%i\n", prefix + 20 - *depth, SYNCTEX_CHAR_BEGIN_VBOX, SYNCTEX_TMP_ITLHVWHD(node));
        //
        *depth = (*depth + 1) % 20;
        if ((N = synctex_node_child(node))) {
            do {
                _synctex_node_dump(N, printer, depth);
            } while ((N = synctex_node_sibling(N)));
        }
        *depth = (*depth + 19) % 20;
        (*printer)("%s%c\n", prefix + 20 - *depth, SYNCTEX_CHAR_END_VBOX);
        break;
    case synctex_node_type_hbox:
        (*printer)("%s%c%s:%i:%i:%i:%i:%i:%i:%i\n", prefix + 20 - *depth, SYNCTEX_CHAR_BEGIN_HBOX, SYNCTEX_TMP_ITLHVWHD(node));
        *depth = (*depth + 1) % 20;
        if ((N = synctex_node_child(node))) {
            do {
                _synctex_node_dump(N, printer, depth);
            } while ((N = synctex_node_sibling(N)));
        }
        *depth = (*depth + 19) % 20;
        (*printer)("%s%c\n", prefix + 20 - *depth, SYNCTEX_CHAR_END_HBOX);
        break;
    case synctex_node_type_void_vbox:
        (*printer)("%s%c:%s:%i:%i:%i:%i:%i:%i:%i\n", prefix + 20 - *depth, SYNCTEX_CHAR_VOID_VBOX, SYNCTEX_TMP_ITLHVWHD(node));
        break;
    case synctex_node_type_void_hbox:
        (*printer)("%s%c:%s:%i:%i:%i:%i:%i\n", prefix + 20 - *depth, SYNCTEX_CHAR_VOID_HBOX, SYNCTEX_TMP_ITLHVWHD(node));
        break;
    case synctex_node_type_kern:
        (*printer)("%s%c:%s:%i:%i:%i:%i:%i:%i\n", prefix + 20 - *depth, SYNCTEX_CHAR_KERN, SYNCTEX_TMP_ITLHV(node), synctex_node_w(node));
        break;
    case synctex_node_type_glue:
        (*printer)("%s%c:%s:%i:%i:%i:%i:%i:%i\n", prefix + 20 - *depth, SYNCTEX_CHAR_GLUE, SYNCTEX_TMP_ITLHV(node), synctex_node_w(node));
        break;
    case synctex_node_type_rule:
        (*printer)("%s%c:%s:%i:%i:%i:%i:%i:%i\n", prefix + 20 - *depth, SYNCTEX_CHAR_RULE, SYNCTEX_TMP_ITLHV(node), synctex_node_w(node));
        break;
    case synctex_node_type_math:
        (*printer)("%s%c:%s:%i:%i:%i:%i:%i:%i\n", prefix + 20 - *depth, SYNCTEX_CHAR_MATH, SYNCTEX_TMP_ITLHV(node), synctex_node_w(node));
        break;
    case synctex_node_type_boundary:
        (*printer)("%s%c:%s:%i:%i:%i:%i:%i:%i\n", prefix + 20 - *depth, SYNCTEX_CHAR_BOUNDARY, SYNCTEX_TMP_ITLHV(node), synctex_node_w(node));
        break;
    case synctex_node_type_box_bdry:
        (*printer)("%sb:%s:%i:%i:%i:%i:%i\n", prefix + 20 - *depth, SYNCTEX_TMP_ITLHV(node));
        break;
#if 0
        case synctex_node_type_input:
        case synctex_node_type_sheet:
        case synctex_node_type_form:
        case synctex_node_type_ref:
        case synctex_node_type_box_bdry:
        case synctex_node_type_proxy:
        case synctex_node_type_proxy_last:
        case synctex_node_type_proxy_vbox:
        case synctex_node_type_proxy_hbox:
        case synctex_node_type_handle:
#endif
    default:
        (*printer)("%sDump unexpected node %s:%i%i\n", prefix + 20 - *depth, synctex_node_isa(node), synctex_node_t(node), synctex_node_l(node));
        break;
    }
}

#undef SYNCTEX_GETTER
#undef SYNCTEX_SETTER
#undef SYNCTEX_INT_GETTER
#undef SYNCTEX_INT_SETTER
#undef SYNCTEX_DECLARE_INT
#undef SYNCTEX_DECLARE_NODEPTR_NO_SETTER
#undef SYNCTEX_DECLARE_NODEPTR_WEAK
#undef SYNCTEX_DECLARE_AS

#endif
#endif
#endif
