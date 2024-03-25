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
local lfs = package.loaded.lfs

print(lfs.currentdir())
print(AUP._VERSION)
print(AUP._DESCRIPTION)

local AUP_units = AUP.units
local exclude = {}
AUP_units:test_currentdir(exclude)

local PL = AUP.PL
local dir = PL.dir
local path = PL.path
local file = PL.file
local seq = PL.seq
local utils = PL.utils
local stringx = PL.stringx


local match = string.match
local join = path.join
local makepath = dir.makepath
local write = file.write
local read = file.read
local splitlines = stringx.splitlines
local printf = utils.printf
local cwd = path.currentdir()
local units = AUP.units
assert(units)
local my_path = units:get_current_tmp_dir()
makepath(my_path)
AUP.pushd_or_raise(my_path)
write ("dump.tex", [==[

]==])

--[[

for name in seq.list {'pdftex', 'luatex', 'xetex'} do
  local p_2 = join(my_path, name)
  makepath(p_2)
  if AUP.pushd(p_2) then
    local _ = AUPEngine(name):synctex(-1):interaction(InteractionMode.nonstopmode):file('../gh78.tex'):run()
    local p_3 = join(p_2, "gh78.synctex")
    local s = assert(read(p_3))
    for _,l in ipairs(splitlines(s)) do
      if match(l, "gh78.tex") then
        printf("%s->\n<%s>\n", p_3, l)
        break
      end
    end
    AUP.popd()
  end
end
path.chdir(cwd)
]]

