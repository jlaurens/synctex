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



local AUP = package.loaded.AUP
local AUP_units = AUP.units
local PL = AUP.PL
local List = PL.List
local PL_path = PL.path
local PL_dir = PL.dir

-- exclude directories in next list
local exclude = List({"fake example"})

local g = AUP.module.SyncTeX.Global("synctex")

local status, code, output, error
status, code, output, error = g:run_option("-v")
assert(status, "Problem with option: -v")
print(output)
status, code, output, error = g:run_option("--version")
assert(status, "Problem with option: --version")
assert(error=="", "Unexpected error")
print(output)
status, code, output, error = g:run_option({"--parse_int_policy", "C"})
assert(status, "Problem with option: --parse_int_policy")
assert(error~="", "Missing error")
status, code, output, error = g:run_option({"--parse_int_policy", "raw1"})
assert(status, "Problem with option: --parse_int_policy")
assert(error~="", "Missing error")
status, code, output, error = g:run_option({"--parse_int_policy", "raw2"})
assert(status, "Problem with option: --parse_int_policy")
assert(error~="", "Missing error")
status, code, output, error = g:run_option({"--parse_int_policy", "raw0"})
assert(status, "Problem with option: --parse_int_policy")
assert(error~="", "Missing error")
print(error)
