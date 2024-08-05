--[[
Copyright (c) 2024 jerome DOT laurens AT u-bourgogne DOT fr
This file is a bridge to the __SyncTeX__ package testing framework.

## License
 
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
 
--]]

--- @type AUP
local AUP = package.loaded.AUP

local List = require"pl.List"

-- exclude directories in next list
local exclude = List({"fake example"})

--- @type AUP.SyncTeX.Global
local g = AUP.SyncTeX.Global("synctex")

local result = g:run_option("-v")
assert(result, "Problem with option: -v")
result:print()
result = g:run_option("--version")
assert(result, "Problem with option: --version")
assert(result.errout=="", "Unexpected error: "..result.errout)
result = g:run_option({"--parse_int_policy", "C"})
assert(result, "Problem with option: --parse_int_policy")
assert(result.errout~="", "Missing error")
result = g:run_option({"--parse_int_policy", "raw1"})
assert(result, "Problem with option: --parse_int_policy")
assert(result.errout~="", "Missing error")
result = g:run_option({"--parse_int_policy", "raw2"})
assert(result, "Problem with option: --parse_int_policy")
assert(result.errout~="", "Missing error")
result = g:run_option({"--parse_int_policy", "raw0"})
assert(result, "Problem with option: --parse_int_policy")
assert(result.errout~="", "Missing error")
