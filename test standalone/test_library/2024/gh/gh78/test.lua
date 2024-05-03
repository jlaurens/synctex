--[===[
Copyright (c) 2008-2024 jerome DOT laurens AT u-bourgogne DOT fr
This file is part of the __SyncTeX__ package testing facilities.

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
 
]===]

print("This test always passes")
print("=======================")

local AUP = package.loaded.AUP
local PL = AUP.PL
local dir = PL.dir
local path = PL.path
local file = PL.file
local seq = PL.seq
local PL_utils = PL.utils
local stringx = PL.stringx

local AUPEngine = AUP.module.engine
local InteractionMode = AUPEngine.InteractionMode

local match = string.match
local join = path.join
local makepath = dir.makepath
local write = file.write
local read = file.read
local splitlines = stringx.splitlines
local printf = PL_utils.printf
local cwd = path.currentdir()

local units = AUP.units

local my_path = join(units:get_current_tmp_dir(), '¡¢£¤¥¦§', '¨©ª«¬­®')
makepath(my_path)
AUP.pushd_or_raise(my_path)
write ("gh78.tex", [==[
A
B
C
\bye
]==])
for name in seq.list {'pdftex', 'luatex', 'xetex'} do
  makepath(name)
  if AUP.pushd(name) then
    local _ = AUPEngine(name):synctex(-1):interaction(InteractionMode.nonstopmode):file('../gh78.tex'):run()
    local s = assert(read("gh78.synctex"))
    for _,l in ipairs(splitlines(s)) do
      if match(l, "gh78.tex") then
        printf("%s->\n<%s>\n", path.join(path.currentdir(), "gh78.synctex"), l)
        break
      end
    end
    AUP.popd()
  end
end
path.chdir(cwd)
