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
local PL_dir = PL.dir
local PL_path = PL.path
local file = PL.file
local PL_seq = PL.seq
local PL_utils = PL.utils
local PL_stringx = PL.stringx

local AUPEngine = AUP.Engine
local InteractionMode = AUPEngine.InteractionMode

local match = string.match
local join = PL_path.join
local makepath = PL_dir.makepath
local write = file.write
local read = file.read
local splitlines = PL_stringx.splitlines
local printf = PL_utils.printf

local units = AUP.units

local my_path = join(units:tmp_dir_current(), '¡¢£¤¥¦§', '¨©ª«¬­®')
makepath(my_path)
local unit = 'gh78'
local unit_tex = unit..'.tex'
local unit_synctex = unit..'.synctex'
AUP.pushd_or_raise(my_path, unit)
write (unit_tex, [==[
A
B
C
\bye
]==])
for name in AUPEngine.tex_all() do
  makepath(name)
  if AUP.pushd(name, 'engine') then
    local _ = AUPEngine(name):synctex(-1):interaction(InteractionMode.nonstopmode):file(PL.path.join('..', unit_tex)):run()
    local s = assert(read(unit_synctex))
    for _,l in ipairs(splitlines(s)) do
      if match(l, unit_tex) then
        printf("%s->\n<%s>\n", PL_path.join(PL_path.currentdir(), unit_synctex), l)
        break
      end
    end
    AUP.popd('engine')
  end
end
AUP.popd(unit)
