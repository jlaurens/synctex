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
local dbg = AUP.dbg
local units = AUP.units

local List    = require"pl.List"
local pl_dir = require"pl.dir"
local pl_path = require"pl.path"

-- exclude directories in next list
local exclude = List({"fake example"})

units:test_currentdir(exclude)

local errors = List()
local Arguments = AUP.Arguments
local arguments = assert(AUP.arguments)
local Engine = AUP.Engine
local InteractionMode = Engine.InteractionMode

local result, status, iter, dir_obj

for name in Engine.tex_all() do
  local entries = assert(arguments:get(name, AUP.Arguments.GetMode.All))
  for _,entry in ipairs(entries) do
    local p = entry.value
    -- if the path is absolute, go to the directory and typeset there
    if pl_path.isabs(p) then
      local d, n = pl_path.splitpath(p)
      if AUP.pushd(d, 'test') then
        local engine = Engine(name):synctex(-1):interaction(InteractionMode.nonstopmode):file(n)
        --- @type AUP.Command.Result
        result = engine:run()
        result:assert_success()
        result:print_stdout()
        if not result.status then
          result:print_errout{}
          units:fail('Typesetting error (cmd: %s)'%{engine:cmd()})
        end
        AUP.popd('test')
      else
        errors:append({"Cannot push to "..d, 2})
      end
    end
  end
end

-- Typeset the files in the directories named after engine names
-- Use cases are not straightforward yet
for name in Engine.tex_all() do
  local engine_dir = pl_path.abspath(name)
  status, iter, dir_obj = pcall(function()
    return pl_path.dir(engine_dir)
  end)
  if status then
    local tmp_dir = pl_path.join(units:tmp_dir_current(), name)
    pl_dir.makepath(tmp_dir)
    dbg:write(10,'tmp_dir: '..tmp_dir)
    if AUP.pushd(tmp_dir, 'tmp_raw') then
      while true do
---@diagnostic disable-next-line: redundant-parameter
        local base = iter(dir_obj)
        if base then
          if pl_path.extension(base)=='.tex' then
            dbg:write(10,"TYPESETTING "..base)
            local p = pl_path.abspath(base, engine_dir)
            local engine = Engine(name):synctex(-1):interaction(InteractionMode.nonstopmode):file(p)
            result = engine:run()
            result:assert_success()
            result:print_stdout()
            if not result.status then
              result:print_errout{}
              units:fail('Typesetting error (cmd: %s)'%{engine:cmd()})
            end
          end
        else
          break
        end
      end
      AUP.popd('tmp_raw')
    end
  end
end
