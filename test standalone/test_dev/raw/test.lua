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
local Arguments = AUP.module.arguments
local arguments = AUP.arguments
local PL_stringx = PL.stringx
local match = string.match
local splitlines = PL_stringx.splitlines
local PL_utils = PL.utils
local printf = PL_utils.printf
local AUPEngine = AUP.module.engine
local InteractionMode = AUPEngine.InteractionMode

local status, _ans, stdout, errout, iter, dir_obj

for engine in AUPEngine.all:iter() do
  local entries = arguments:get(engine, Arguments.GetMode.All)
  for _,entry in ipairs(entries) do
    local p = entry.value
    -- if the path is absolute, go to the directory and typeset there
    if PL.path.isabs(p) then
      local d, n = PL.path.splitpath(p)
      if AUP.pushd(d) then
        status, _ans, stdout, errout = AUPEngine(engine):synctex(-1):interaction(InteractionMode.nonstopmode):file(n):run()
        print(stdout)
        if not status then
          print("ERROR:")
          print(errout)
          AUP_units:fail('Typesetting error')
        end
        AUP.popd()
      else
        errors:append({"Cannot push to "..d, 2})
      end
    end
  end
end

-- Typeset the files in the directories named after engine names
-- Use cases are not straightforward yet
for engine in AUPEngine.all:iter() do
  local engine_dir = PL.path.abspath(engine)
  status, iter, dir_obj = pcall(function()
    return PL.path.dir(engine_dir)
  end)
  if status then
    local tmp_dir = PL.path.join(AUP_units:get_current_tmp_dir(), engine)
    PL.path.mkdir(tmp_dir)
    dbg:write(10,'tmp_dir: '..tmp_dir)
    if AUP.pushd(tmp_dir) then
      while true do
        local name = iter(dir_obj)
        if name then
          if PL.path.extension(name)=='.tex' then
            dbg:write(10,"TYPESETTING "..name)
            local p = PL.path.abspath(name, engine_dir)
            status, _ans, stdout, errout = AUPEngine(engine):synctex(-1):interaction(InteractionMode.nonstopmode):file(PL.path.relpath(p)):run()
            print(stdout)
            if not status then
              print("ERROR:")
              print(errout)
              AUP_units:fail('Typesetting error')
            end
              end
        else
          break
        end
      end
      AUP.popd()
    end
  end
end
