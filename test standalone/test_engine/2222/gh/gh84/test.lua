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

local pl_dir  = require"pl.dir"
local pl_path = require"pl.path"
local pl_file = require"pl.file"
local pl_utils = require"pl.utils"
local pl_stringx = require"pl.stringx"

local match = string.match
local join = pl_path.join
local read = pl_file.read
local splitlines = pl_stringx.splitlines
local printf = pl_utils.printf

--- @class AUP
local AUP = package.loaded.AUP

local dbg = AUP.dbg

dbg:write(1, "Testing mathsurround")

local Command = AUP.Command
local Engine = AUP.Engine
local InteractionMode = Engine.InteractionMode

local units = AUP.units

local cwd = assert(lfs.currentdir())

AUP.pushd_or_raise(units:tmp_dir_current(), 'tmp_gh84')
AUP.br{label='FIRST ROUND', ch='='}
for engine in Engine.tex_all() do
  AUP.br{label='ENGINE: '..engine}
  local base = 'gh84-'..engine
  local source = join(cwd, base..".tex")
  dbg:write(1, Command.which(engine, Command.tex_bin_get(), true))
  --- @type AUP.Command.Result
  local result = Engine(engine):synctex(-1):interaction(InteractionMode.nonstopmode):file(source):run()
  result:assert_success()
  for i,l in ipairs(splitlines(result.stdout)) do
    if match(l, "Synchronize ERROR") then
      printf("Unexpected at line %i: <%s>\n", i, l)
    end
  end
  local s = read(base..'.synctex')
  if s ~= nil then
    local function f(...)
      for _,request in ipairs({...}) do
        if not s:find(request) then
          AUP.br{label=base..'.synctex'}
          units:fail('Wrong '..base..'.synctex (no "'..request..'")')
          print(s)
          return
        end
      end
    end
    if engine:find('lua') then
      f('$1,7:1111,', '$1,18:2222,')
    else
      f('$1,7:2222,')
    end
  else
    units:fail('dev: MISSING '..base..'.synctex')
  end
end
AUP.popd_or_raise('tmp_gh84')

-- There is a second round with smart links to input files
-- TODO: remove the first round if this round works on windows
AUP.br{label='SECOND RUN', ch='='}
AUP.pushd_or_raise(units:tmp_dir_current(), 'tmp_gh84_2')
for name in Engine.tex_all() do
  AUP.br{label='ENGINE: '..name}
  pl_dir.makepath(name)
  AUP.pushd_or_raise(name, 'engine')
  local base = 'gh84-'..name
  local source = join(cwd, base..'.tex')
  base = base..'-ln-s'
  local dest = join(assert(lfs.currentdir()), base..'.tex')
  os.remove(dest)
  lfs.link(source, dest, true)
  local engine = Engine(name):synctex(-1):interaction(InteractionMode.nonstopmode):file(dest)
  --- @type AUP.Command.Result
  local result = engine:run()
  result:assert_success()
  for i,l in ipairs(splitlines(result.stdout)) do
    if match(l, 'Synchronize ERROR') then
      printf('Unexpected at line %i: <%s>\n', i, l)
    end
  end
  local s = read(base..'.synctex')
  assert(s, 'No .synctex availeble (cmd: %s)'%{engine:cmd()})
  local function f(...)
    for _,request in ipairs({...}) do
      if not s:find(request) then
        AUP.br{label=base..'.synctex'}
        units:fail("Wrong "..base.." (no '"..request.."')")
        print(s)
        return
      end
    end
  end
  if string.find(name, 'lua') then
    f('$1,7:1111,', '$1,18:2222,')
  else
    f('$1,7:2222,')
  end
  AUP.popd_or_raise('engine')
end
AUP.popd_or_raise('tmp_gh84_2')
