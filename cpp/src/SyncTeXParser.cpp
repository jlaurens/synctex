/**
 * SyncTeXParser.cpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

#include "SyncTeXParser.hpp"
#include "SyncTeXManagerI9on.hpp"
#include "SyncTeXProxy.hpp"

namespace SyncTeXpp {

Parser_p Parser::make_p(Manager::I9on_p p) {
    if (p) {
        auto reader_p = p->_reader_p;
        if (reader_p) {
            return Parser_p(new Parser(p, reader_p));
        }
    }
    return Parser_p(nullptr);
}

template<typename T>
bool Parser::content_tlc_hv_WHD(bool fit, int &error_count) {
    tlc_hv_WHD_s s;
    if (Status::Done < _reader_p->decode_tlc_hv_WHD(s)) {
        SYNCTEX_COVERAGE(decode_tlc_hv_WHD ERROR);
        ++error_count;
        return false;
    }
    insert_other(T::make_p(
        s.t, s.l, s.c,
        s.h, s.v,
        s.W, s.H, s.D
    ), fit);
    return true;
}

template<typename T>
bool Parser::content_tlc_hv(int &error_count) {
    tlc_hv_s s;
    if (Status::Done < _reader_p->decode_tlc_hv(s)) {
        ++error_count;
        return false;
    }
    insert_other(T::make_p(
        s.t, s.l, s.c,
        s.h, s.v
    ), true);
    return true;
}

Status Parser::parse_sheet(
    Manager::I9on_p i_p,
    int &error_count
) {
    auto r_p = i_p->_reader_p;
    if (!r_p) {
        ++error_count;
        return Status::ErrorLogical;
    }
    Status status;
    int t = 0;
    if (Status::Done < (status = r_p->decode(t))
        || Status::Done < (status = r_p->require_endl())) {
        ++error_count;
        return status;
    }
    auto p = Parser::make_p(i_p);
    p->_sheet_p = Sheet::make_p(t);
    p->_above_p = p->_sheet_p;
    i_p->_sheet_p_by_tag[t] = p->_sheet_p;
    return p->parse_content(error_count);
}

Status Parser::parse_form(
    Manager::I9on_p i_p,
    int &error_count
) {
    auto r_p = i_p->_reader_p;
    if (!r_p) {
        ++error_count;
        return Status::ErrorLogical;
    }
    Status status;
    int t = 0;
    if (Status::Done < (status = r_p->decode(t))
        || Status::Done < r_p->require_endl()) {
        ++error_count;
        return Status::ErrorDataMissing;
    }
    auto p = Parser::make_p(i_p);
    p->_form_p = Form::make_p(t);
    p->_above_p = p->_form_p;
    i_p->_form_p_by_tag[t] = p->_form_p;
    return p->parse_content(error_count);
}

Status Parser::parse_postamble(
    Manager::I9on_p i_p,
    int &error_count
) {
    auto r_p = i_p->_reader_p;
    if (!r_p) {
        ++error_count;
        return Status::ErrorLogical;
    }
    Status status;
    
    // Skip the end of line after "Postamble:"
    if (Status::Done < (status = r_p->require_endl())) {
        return status;
    }
    
    // Look for the "Count:" record
    while (true) {
        if (Status::EndOfData < (status = r_p->read_string("Count:"))) {
            return status; /*  forward the error */
        } else if (Status::Done == status) {
            break; /* Found Count: */
        } else {
            // Not found, skip this line and try again
            if (Status::Done < (status = r_p->require_endl())) {
                return Status::Done; /* The EndOfData is found */
            }
            continue;
        }
    }
    
    // Decode the count
    if (Status::EndOfData < (status = r_p->decode(i_p->_count))) {
        return status;
    }
    
    // Require end of line after the count
    if (Status::Done < (status = r_p->require_endl())) {
        return Status::Done;
    }
    
    return Status::Done;
}

// #define SYNCTEX_READ(WHAT)  \
//     (Status::Done == (_status = _reader_p->read_char(Prefix::WHAT)) && SYNCTEX_COVERAGE(#WHAT) && SYNCTEX_COVERAGE_PRINT(#WHAT))
#define SYNCTEX_READ(WHAT)  \
    (Status::Done == (_status = _reader_p->read_char(Prefix::WHAT)))

Status Parser::parse_content(
    int &error_count
) {
    while(true) {
        /*  Either inside a form or a sheet.
        *  - in a sheet, "{" is not possible, only (v)boxes and "}" at top level.
        *  - in a form, "{" is not possible, only boxes, "<" and balancing ">".
        *  - in a box, the unique possibility is '<', '[', '(' and balancing ']', ')' or ">".
        *  We still keep the '(' possibility for a sheet, because that does not cost too much.
        *  We must also consider void boxes as children.
        */
        if (SYNCTEX_READ(BEGIN_VBOX)) {
            tlc_hv_WHD_s s;
            if (Status::Done < _reader_p->decode_tlc_hv_WHD(s)) {
                SYNCTEX_COVERAGE(decode_tlc_hv_WHD ERROR);
                ++error_count;
            } else {
                insert_vbox(s);
            }
        } else if (SYNCTEX_READ(BEGIN_HBOX)) {
            tlc_hv_WHD_s s;
            if (Status::Done < _reader_p->decode_tlc_hv_WHD(s)) {
                SYNCTEX_COVERAGE(decode_tlc_hv_WHD ERROR);
                ++error_count;
            } else {
                insert_hbox(s);
            }
        } else if (SYNCTEX_READ(VOID_VBOX)) {
            content_tlc_hv_WHD<VoidVBox>(false, error_count);
        } else if (SYNCTEX_READ(VOID_HBOX)) {
            content_tlc_hv_WHD<VoidHBox>(true, error_count);
        } else if (SYNCTEX_READ(RULE)) {
            content_tlc_hv_WHD<Rule>(false, error_count);
        } else if (SYNCTEX_READ(KERN)) {
            tlc_hv_W_s s;
            if (Status::Done < _reader_p->decode_tlc_hv_W(s)) {
                ++error_count;
            } {
                Below_p p = Kern::make_p(
                    s.t, s.l, s.c,
                    s.h, s.v,
                    s.W
                );
                insert(p);
                if (!_form_p) _i9on_p->input_push_back(p);
                _above_p->fit_to_wnes_V(p);
                _below_p = p;
            }
        } else if (SYNCTEX_READ(GLUE)) {
            content_tlc_hv<Glue>(error_count);
        } else if (SYNCTEX_READ(MATH)) {
            content_tlc_hv<Math>(error_count);
        } else if (SYNCTEX_READ(BOUNDARY)) {
            content_tlc_hv<Bdry>(error_count);
        } else if (SYNCTEX_READ(FORM_REF)) {
            t_hv_s s;
            if (Status::Done < _reader_p->decode_t_hv(s)) {
                ++error_count;
            } else {
                auto p = Ref::make_p(
                    s.t,
                    s.h, s.v
                );
                insert(p);
                _ref_p_v.push_back(p);
                _below_p = p;
            }
        } else if (SYNCTEX_READ(CHARACTER)) {
            if (Status::Done < _reader_p->require_endl()) {
                return Status::Error;
            }
        } else if (SYNCTEX_READ(COMMENT)) {
            if (Status::Done < _reader_p->require_endl()) {
                return Status::Error;
            }
        } else if (SYNCTEX_READ(END_VBOX)) {
            if (_above_p->isVBox()) {
                PairBelow_p pp = _above_p->make_bdries();
                _i9on_p->input_push_back(pp.first);
                _i9on_p->input_push_back(pp.second);
                go_above();
                if (Status::Done < _reader_p->require_endl()) {
                    return _status;
                }
            } else {
                ++error_count;
                _reader_p->skip_endl();
            }
        } else if (SYNCTEX_READ(END_HBOX)) {
            if (_above_p->isHBox()) {
                if (_sheet_p) _sheet_p->push_back(_above_p);
                /*  Update the mean line number */
                Below_p node_p = _above_p->get_below_p();
                Below_p right_p = node_p;
                if (right_p) {
                    unsigned int node_weight = 0;
                    unsigned int cumulated_line_numbers = 0;
                    do {
                        if (right_p->isHBox()) {
                            auto weight = right_p->weight();
                            if (weight) {
                                node_weight += weight;
                                cumulated_line_numbers += node_p->mean_l() * weight;
                            } else {
                                ++node_weight;
                                cumulated_line_numbers += node_p->mean_l();
                            }
                        } else {
                            ++node_weight;
                            cumulated_line_numbers += node_p->l();
                        }
                    } while ((right_p = right_p->get_right_p()));
                    _above_p->set_mean_l((cumulated_line_numbers + node_weight / 2) / node_weight);
                    _above_p->set_weight(node_weight);
                } else {
                    _above_p->set_mean_l(_above_p->l());
                    _above_p->set_weight(1);
                }
                // Insert a closing BoxBdry
                // The tlc info comes from the first left node that is not a Ref
                PairBelow_p pp = _above_p->make_bdries();
                _i9on_p->input_push_back(pp.first);
                _i9on_p->input_push_back(pp.second);
                go_above();
                if (Status::Done < (_status = _reader_p->skip_endl())) {
                    // std::cerr << "Incomplete container." << std::endl;
                    return _status;
                }
            } else {
                ++error_count;
            }
        } else if (SYNCTEX_READ(ANCHOR)) {
            if (Status::Done < (_status = _reader_p->skip_endl())) {
                ++error_count;
                // std::cerr << "Missing anchor." << std::endl;
                return _status;
            }
        } else if (SYNCTEX_READ(END_SHEET)) {
            if (_sheet_p) {
                int t = 0;
                if (Status::Done < (_status = _reader_p->decode(t))) {
                    SYNCTEX_COVERAGE("NO TAG");
                    ++error_count;
                } else if (t != _sheet_p->t()) {
                    auto msg = "BAD TAG ⟨actual⟩!=⟨expected⟩ :" + std::to_string(t)+"!="+std::to_string(_sheet_p->t());
                    SYNCTEX_R_COVERAGE(msg);
                    ++error_count;
                }
                if (_above_p != _sheet_p) {
                    SYNCTEX_COVERAGE("ABOVE!=SHEET");
                    ++error_count;
                    _status = Status::ErrorDataContent;
                }
                if (Status::Done < (_status = _reader_p->require_endl())) {
                    SYNCTEX_COVERAGE("NO EOL");
                    ++error_count;
                }
                _above_p.reset();
                _sheet_p.reset();
                auto s = post_process_refs(error_count);
                if (Status::Done < s) {
                    _status = s;
                    SYNCTEX_COVERAGE("Bad ref tag");
                }
            } else {
                SYNCTEX_COVERAGE("NO >");
                _reader_p->advance(-1);
                ++error_count;
            }
            return _status;
        } else if (SYNCTEX_READ(BEGIN_FORM)) {
            if (Status::Done < (_status = Parser::parse_form(_i9on_p, error_count))) {
                ++error_count;
                return _status;
            }
            continue;
        } else if (SYNCTEX_READ(END_FORM)) {
            if (_form_p) {
                int t = 0;
                if (Status::Done < (_status = _reader_p->decode(t))) {
                    ++error_count;
                } else if (t != _form_p->t()) {
                    ++error_count;
                }
                if (Status::Done < (_status = _reader_p->require_endl())) {
                    // std::cerr << "Missing end of container." << std::endl;
                    ++error_count;
                }
                if(_above_p != _form_p) {
                    SYNCTEX_COVERAGE("ABOVE!=FORM");
                    ++error_count;
                    _status = Status::ErrorDataContent;
                }
                return _status;
            }
            ++error_count;
            _reader_p->skip_endl();
            return Status::ErrorLogical;
        } else {
            SYNCTEX_COVERAGE(OTHER);
            int foo;
            // int size = 0;
            // std::cerr << "IGNORED: " << _reader_p->data(size) << std::endl;
            if (Status::Done < _reader_p->require_endl()) {
                SYNCTEX_COVERAGE(require_endl ERROR);
                ++error_count;
                return Status::ErrorDataMissing;
            }
        }
    }
}
#undef SYNCTEX_READ

Status Parser::post_process_refs(int &error_count) {
    Status s = Status::Done;
    for (const auto& ref_p : _ref_p_v) {
        auto above_p = ref_p->get_above_p();
        if (above_p) {
            auto left_p = ref_p->get_left_p();
            auto right_p = ref_p->get_right_p();
            auto it = _i9on_p->_form_p_by_tag.find(ref_p->t());
            if (it != _i9on_p->_form_p_by_tag.end()) {
                auto target_p = it->second->get_below_p();
                /*  The target is a single node (box)
                 *  with children and no siblings. */
                auto prx = Proxy::make_p(ref_p, target_p);
                if (prx) {
                    /*  Insert this proxy instead of ref. */
                    prx->set_right_p(right_p);
                    if(left_p) {
                        left_p->set_right_p(prx);
                    } else {
                        ref_p->set_right_p(nullptr);
                        above_p->set_below_p(prx);
                    }
                } else /*  simply remove ref */ {
                    ++error_count;
                    s = Status::ErrorDataBadRefTag;
                    if(left_p) {
                        left_p->set_right_p(right_p);
                    } else {
                        above_p->set_below_p(right_p);
                    }
                }
            } else {
                // There is no form for that given tag
                ++error_count;
                s = Status::ErrorDataBadRefTag;
            }
        } else {
            ++error_count;
            s = Status::ErrorDataBadRefTag;
        }
    }
    return s;
}

};
