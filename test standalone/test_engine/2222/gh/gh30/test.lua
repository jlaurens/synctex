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

local AUP = package.loaded.AUP

local dbg = AUP.dbg

dbg:write(1, "Testing unknown node type (2024)")

local pl_path = require"pl.path"
local pl_stringx = require"pl.stringx"

local FmtUtil = AUP.FmtUtil
local Engine = AUP.Engine
local InteractionMode = Engine.InteractionMode

local join = pl_path.join
local splitlines = pl_stringx.splitlines

local units = AUP.units

local cwd = assert(lfs.currentdir());

local unit = 'gh30'
local unit_tmp = unit..'_tmp'
AUP.pushd_or_raise(units:tmp_dir_current(), unit_tmp)
for name in Engine.tex_all() do
  if name:find('lua') then
    local base = unit..'_'..name
    local source = join(cwd, base..".tex")
    local engine = Engine(name):synctex(-1)
    engine = Engine:interaction(InteractionMode.nonstopmode):file(source)
    ---@type AUP.Command.Result
    local result = engine:run()
    result:assert_success()
    for _,l in ipairs(splitlines(result.stdout)) do
      if l:find("Fatal format file error; I'm stymied") then
        print("Bad format")
        result = FmtUtil():sys():byengine(name):remake_when_older()
        assert(result.status)
        result:print()
        result = engine:run()
        result:assert_success()
      end
    end
    AUP.br{}
    if not result.stdout:find('Synchronize ERROR: unknown node type 11') then
      units:fail("No more error")
      result:print_stdout()
    end
  end
end
AUP.popd(unit_tmp)
