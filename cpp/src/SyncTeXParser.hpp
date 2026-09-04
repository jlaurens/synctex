/**
 * SyncTeXParser.hpp
 * 
 * This file is part of the SyncTeX library.
 * It defines the class for representing nodes in a SyncTeX tree.
 * 
 * Converted from C to C++ and wrapped in the SyncTeX namespace.
 * Original copyright (c) 2026 jerome DOT laurens AT ube DOT fr
 */

/**
 * @brief Parser of the SyncTeX data.
 *
 */

#ifndef SYNCTEX_PARSER_HPP
#define SYNCTEX_PARSER_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <functional>

#include "SyncTeXManager.hpp"
#include "SyncTeXManagerI9on.hpp"
#include "SyncTeXReader.hpp"
#include "SyncTeXTree.hpp"
#include "SyncTeXNoBox.hpp"
#include "SyncTeXSheet.hpp"
#include "SyncTeXBox.hpp"
#include "SyncTeXForm.hpp"
#include "SyncTeXInput.hpp"

namespace SyncTeXpp {

class Parser;
using Parser_p = std::shared_ptr<Parser>;

class Parser
{
    Manager::I9on_p _i9on_p;
    Reader_p        _reader_p;
    Sheet_p         _sheet_p;
    Form_p          _form_p;
    Above_p         _above_p;
    Below_p         _below_p;
    Ref_v           _ref_p_v;
    Status          _status = Status::Done;
    
    protected:
    Parser(
        Manager::I9on_p i_p,
        Reader_p r_p
    ): _i9on_p(i_p), _reader_p(r_p) {}

    public:
    static Parser_p make_p(Manager::I9on_p i_p);
    /**
     * @brief Parse a Sheet
     * 
     * Can call itself because forms are allowed inside other forms.
     * However, the depth is reasonnably low due to the nature of forms.
     * 
     * @param i_p A private mabager implementation
     * @param error_count The number of errors
     * @return Status Done when everything got well
     * @return Status EndOfData when the end of data has been reached
     * @return Status Error when an error occurred
     */
    static Status parse_sheet(
        Manager::I9on_p i_p,
        int &error_count
    );
    /**
     * @brief Parse a form
     * 
     * Can call itself because forms are allowed inside other forms.
     * However, the depth is reasonnably low due to the nature of forms.
     * 
     * @param i_p A private mabager implementation
     * @param error_count The number of errors
     * @return Status Done when everything got well
     * @return Status EndOfData when the end of data has been reached
     * @return Status Error when an error occurred
     */
    static Status parse_form(
        Manager::I9on_p i_p,
        int &error_count
    );
    /**
     * @brief Parse the postamble section
     * 
     * The postamble contains the record count and possibly post scriptum data.
     * 
     * @param i_p A private manager implementation
     * @param error_count The number of errors
     * @return Status Done when everything went well
     * @return Status EndOfData when the end of data has been reached
     * @return Status Error when an error occurred
     */
    static Status parse_postamble(
        Manager::I9on_p i_p,
        int &error_count
    );
    
    template<typename T>
    bool content_tlc_hv(int &error_count);

    template<typename T>
    bool content_tlc_hv_WHD(bool, int &error_count);

    void insert_vbox(tlc_hv_WHD_s &s) {
        auto v_p = VBox::make_p(
            s.t, s.l, s.c,
            s.h, s.v,
            s.W, s.H, s.D
        );
        insert(v_p);
        _above_p = v_p;
        _below_p.reset();
    }
    void insert_hbox(tlc_hv_WHD_s &s) {
        auto h_p = HBox::make_p(
            s.t, s.l, s.c,
            s.h, s.v,
            s.W, s.H, s.D
        );
        insert(h_p);
        _above_p->fit_to_wnes_V(h_p);
        _above_p = h_p;
        _below_p.reset();
    }
    void insert_other(Below_p p, bool fit) {
        insert(p);
        if (!_form_p) _i9on_p->input_push_back(p);
        if (fit) _above_p->fit_to_wnes_V(p);
        _below_p = p;
    }
    void go_above() {
        _below_p = _above_p;
        _above_p = _below_p->get_above_p();
        _above_p->fit_to_wnes_V(_below_p);
    }

    protected:
    void insert(Below_p p) {
        if (_above_p) {
            _above_p->set_below_p(p);
        } else /* if (_below_p) */ {
            _below_p->set_right_p(p);
        }
    }
    /**
     * @brief Parse the content of a form or a sheet
     * 
     * Can call itself because forms are allowed inside other forms.
     * However, the depth is reasonnably low due to the nature of forms.
     * 
     * @param error_count The number of errors
     * @return Status Done when everything got well
     * @return Status EndOfData when the end of data has been reached
     * @return Status Error when an error occurred
     */
    Status parse_content(int &error_count);

    private:
    Status post_process_refs(int &error_count);

    friend class Manager;
    SYNCTEX_TEST_VISITOR

};

};
#endif // SYNCTEX_PARSER_HPP

#if 0
STRUCTURE

The element structure of a synctex file is a list of text line records as follows. ‘*’, ‘+’, and ‘?’ have their usual EBNF meanings: ‘*’ means zero or more, ‘+’ means one or more, and ‘?’ means zero or one (i.e., optional).

<SyncTeX> ::= (The whole contents in 4 sections)
<Preamble>
<Content>
<Postamble>
<Post Scriptum>

Each section starts with the first occurrence of a sectioning line, and ends with the next section, if any. In the following definitions, we do not mention the section ending condition.
The preamble

<Preamble> ::=
"SyncTeX Version:" <Version Number> <EOL>
<Input Line>*
"Magnification:" <TeX magnification> <EOL>
"Unit:" <unit in scaled point> <EOL>
"X Offset:" <horizontal offset in scaled point> <EOL>
"Y Offset:" <vertical offset in scaled point> <EOL>

<Input Line> ::= "Input:" <tag> ":" <File Name> <EOL>
The content

<Content> ::=
<byte offset record>
"Content:" <EOL>
(<Form(k)>|<Input Line>)*
<sheet(1)>
(<Form(k)>|<Input Line>)*
<sheet(2)>
(<Form(k)>|<Input Line>)*
...
<sheet(N)>
(<Form(k)>|<Input Line>)*
<byte offset record> ::= "!" <byte offset> <end of record>
<sheet(n)> ::=
<byte offset record>
"{" <the integer n> <end of record>
<vbox section>|<hbox section>
<byte offset record>
"}" <the integer n> <end of record>
<Form> ::=
<byte offset record>
"<" <form tag> <end of record>
<vbox section>|<hbox section>
<byte offset record>
">" <end of record>
<form tag> ::= <integer>
Forms are available with pdfTeX. All the numbers are integers encoded using the decimal representation with "C" locale. The <box content> describes what is inside a box. It is either a vertical or horizontal box, with some records related to glue, kern or math nodes.

<box content> ::=
<vbox section>|<hbox section>
|<void vbox record>|<void hbox record>
|<current record>|<glue record>|<kern record>
|<math record>|<form ref record>|<Form>
<vbox section> ::=
"[" <link> ":" <point> ":" <size> <end of record>
<box content>*
"]" <end of record>
<hbox section> ::=
( <link> ":" <point> ":" <size> <end of record>
<box content>*
")" <end of record>
Void boxes:

<void vbox record> ::= "v" <link> ":" <point> ":" <size> <end of record>
<void hbox record> ::= "h" <link> ":" <point> ":" <size> <end of record>
<link> ::= <tag> "," <line>( "," <column>)?
<point> ::= <full point>|<compressed point>
<full point> ::= <integer> "," <integer>
<compressed point> ::= <integer> ",="
<line> ::= <integer>
<column> ::= <integer>
<size> ::= <Width> "," <Height> "," <Depth>
<Width> ::= <integer>
<Height> ::= <integer>
<Depth> ::= <integer>
The forthcoming records are basic one liners.

<current record> ::= "x" <link> ":" <point> <end of record>
<kern record> ::= "k" <link> ":" <point> ":" <Width> <end of record>
<glue record> ::= "g" <link> ":" <point> <end of record>
<math record> ::= "$" <link> ":" <point> <end of record>
<form ref record> ::= "f" <form tag> ":" <point> <end of record>
The postamble

The postamble closes the file If there is no postamble, it means that the typesetting process did not end correctly.

<Postamble>::=
<byte offset record>
"Count:" <Number of records> <EOL>
The post scriptum

The post scriptum contains material possibly added by 3rd parties. It allows one to append some transformation (shift and magnify). Typically, one applies a dvi to pdf filter with offset options and magnification, then he appends the same options to the synctex file, for example

synctex update -o foo.pdf -m 0.486 -x 9472573sp -y 13.3dd source.dvi
<Post Scriptum>::=
<byte offset record>
"Post Scriptum:" <EOL>
"Magnification:" <number> <EOL> (Set additional magnification)
"X Offset:" <dimension> <EOL> (Set horizontal offset)
"Y Offset:" <dimension> <EOL> (Set vertical offset)
This second information will override the offset and magnification previously available in the preamble section. All the numbers are encoded using the decimal representation with "C" locale.
USAGE

The <current record> is used to compute the visible size of hbox's. The byte offset is an implicit anchor to navigate the synctex file from sheet to sheet. The second coordinate of a compressed point has been replaced by a "=" character which means that it is the second coordinate of the last full point available above.
S
#endif