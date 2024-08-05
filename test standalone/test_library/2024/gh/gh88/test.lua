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

local pl_dir = require"pl.dir"
local pl_path = require"pl.path"
local pl_file = require"pl.file"
local pl_utils = require"pl.utils"
local pl_stringx = require"pl.stringx"
local List = require"pl.List"

local join = pl_path.join
local makepath = pl_dir.makepath
local write = pl_file.write
local read = pl_file.read
local splitlines = pl_stringx.splitlines
local printf = pl_utils.printf

--- @type AUP
local AUP = package.loaded.AUP

local Engine = AUP.Engine
local InteractionMode = Engine.InteractionMode

local units = AUP.units
assert(units)

local my_path = join(units:tmp_dir_current(), '¡¢£¤¥¦§', '¨©ª«¬­®')
makepath(my_path)
local unit = 'gh88'
local unit_tex = unit..'.tex'
local unit_synctex = unit..'.synctex'
AUP.pushd_or_raise(my_path, unit)
write (unit_tex, [==[
\def\bb{\hbox to 1cm{$1$ }}
\def\my{\bb\bb\bb\bb\bb\bb\bb\bb\bb\bb}
\def\My{\my\my\my\my\my\my\my\my\my\my}
\def\MY{\My\My\My\My\My\My\My\My\My\My}
\def\my{\bb}
\def\My{\my}
\def\MY{\My}
\MY\MY\MY\MY\MY\MY\MY\MY\MY\MY
\bye
]==])
for name in List{'pdftex'}:iter() do
  makepath(name)
  if AUP.pushd(name, 'engine') then
    local _ = Engine(name):synctex(-1):interaction(InteractionMode.nonstopmode):file(pl_path.join('..', unit_tex)):run()
    local s = assert(read(unit_synctex))
    for _,l in ipairs(splitlines(s)) do
      if l:match(unit_tex) then
        printf("%s->\n<%s>\n", pl_path.join(pl_path.currentdir(), unit_synctex), l)
        break
      end
    end
    AUP.popd('engine')
  end
end
AUP.popd(unit)
