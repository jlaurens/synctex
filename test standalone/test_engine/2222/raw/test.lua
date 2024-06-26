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
local dbg = AUP.dbg
local AUP_units = AUP.units
local PL = AUP.PL
local List = PL.List

-- exclude directories in next list
local exclude = List({"fake example"})

AUP_units:test_currentdir(exclude)

local errors = PL.List()
local AUPArguments = AUP.Arguments
local arguments = AUP.arguments
local AUPEngine = AUP.Engine
local InteractionMode = AUPEngine.InteractionMode

local result, status, iter, dir_obj

for name in AUPEngine.tex_all() do
  local entries = arguments:get(name, AUPArguments.GetMode.All)
  for _,entry in ipairs(entries) do
    local p = entry.value
    -- if the path is absolute, go to the directory and typeset there
    if PL.path.isabs(p) then
      local d, n = PL.path.splitpath(p)
      if AUP.pushd(d, 'test') then
        local engine = AUPEngine(name):synctex(-1):interaction(InteractionMode.nonstopmode):file(n)
        result = engine:run()
        result:print_stdout()
        if not result.status then
          result:print_errout()
          AUP_units:fail('Typesetting error (cmd: %s)'%{engine:cmd()})
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
for name in AUPEngine.tex_all() do
  local engine_dir = PL.path.abspath(name)
  status, iter, dir_obj = pcall(function()
    return PL.path.dir(engine_dir)
  end)
  if status then
    local tmp_dir = PL.path.join(AUP_units:tmp_dir_current(), name)
    PL.dir.makepath(tmp_dir)
    dbg:write(10,'tmp_dir: '..tmp_dir)
    if AUP.pushd(tmp_dir, 'tmp_raw') then
      while true do
        local base = iter(dir_obj)
        if base then
          if PL.path.extension(base)=='.tex' then
            dbg:write(10,"TYPESETTING "..base)
            local p = PL.path.abspath(base, engine_dir)
            local engine = AUPEngine(name):synctex(-1):interaction(InteractionMode.nonstopmode):file(p)
            result = engine:run()
            result:print_stdout()
            if not result.status then
              result:print_errout()
              AUP_units:fail('Typesetting error (cmd: %s)'%{engine:cmd()})
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
