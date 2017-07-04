/*
 Copyright (c) 2008-2017 jerome DOT laurens AT u-bourgogne DOT fr
 
 This file is part of the SyncTeX package.
 
 Latest Revision: Thu Mar  9 21:26:27 UTC 2017
 
 Version: 1.19
 
 See synctex_parser_readme.txt for more details
 
 License:
 --------
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE
 
 Except as contained in this notice, the name of the copyright holder
 shall not be used in advertising or otherwise to promote the sale,
 use or other dealings in this Software without prior written
 authorization from the copyright holder.
 
 Nota Bene:
 ----------
 If you include or use a significant part of the synctex package into a software,
 I would appreciate to be listed as contributor and see "SyncTeX" highlighted.
 
 */

#include "synctex_parser_private.h"
#include "synctex_parser_utils.h"

int synctex_parser_test_all() {
    int TC = 0;
    synctex_scanner_p scanner = synctex_scanner_new();
    /*  Here we assume that int are smaller than void * */
#if 1
    TC += synctex_test_input(scanner);
    TC += synctex_test_proxy(scanner);
    TC += synctex_test_page(scanner);
    TC += synctex_test_handle(scanner);
    TC += synctex_scanner_free(scanner);
    scanner = synctex_scanner_new();
    TC += synctex_test_display_query(scanner);
/*
 */
#endif
    TC += synctex_scanner_free(scanner);
    TC += synctex_test_sheet_1();
    TC += synctex_test_sheet_2();
    TC += synctex_test_charindex();
    TC += synctex_test_form();
/*
 */
    return TC;
}
