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

---@type LuaFileSystem
local lfs = lfs

local pl_dir      = require"pl.dir"
local pl_path     = require"pl.path"
local pl_utils    = require"pl.utils"
local pl_stringx  = require"pl.stringx"

--- @type AUP
local AUP = package.loaded.AUP

local dbg = AUP.dbg

dbg:write(1, "Testing minimal")

local Engine = AUP.Engine
local InteractionMode = Engine.InteractionMode

local makepath    = pl_dir.makepath
local join        = pl_path.join
local splitlines  = pl_stringx.splitlines
local readfile    = pl_utils.readfile
local printf      = pl_utils.printf

local units = AUP.units

local cwd = assert(lfs.currentdir());

AUP.pushd_or_raise(units:tmp_dir_current(), 'tmp_minimal')

for name in Engine.tex_all() do
  local base = 'minimal-'..name
  local source = join(cwd, base..".tex")
  makepath(name)
  AUP.pushd_or_raise(name, base)
  local result = Engine(name):synctex(-1):interaction(InteractionMode.nonstopmode):file(source):run()
  result:assert_success()
  for i,l in ipairs(splitlines(result.stdout)) do
    if l:match("Synchronize ERROR") then
      printf("Unexpected at line %i: <%s>\n", i, l)
    end
  end
  result:print()
  print(base..".synctex")
  local s = readfile(base..".synctex")
  if s == nil then
    AUP.units:fail('MISSING '..base..".synctex")
  end
  AUP.popd(base)
end
AUP.popd_or_raise('tmp_minimal')
