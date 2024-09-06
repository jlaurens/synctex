--[==[
Copyright (c) 2024 jerome DOT laurens AT u-bourgogne DOT fr
This file is part of the __SyncTeX__ package testing framework.

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
 
--]==]

---@type LuaFileSystem
local lfs = lfs

local pl_class      = require"pl.class"
local List          = require"pl.List"
local pl_utils      = require"pl.utils"
local pl_stringx    = require"pl.stringx"
local pl_dir        = require"pl.dir"
local pl_seq        = require"pl.seq"
local pl_path       = require"pl.path"
local assert_string = pl_utils.assert_string

--- @class AUP
local AUP = package.loaded.AUP

local Command = AUP.Command

local K = AUP.K

--- @class AUP.Engine: AUP.Command
local Engine = pl_class(Command)

AUP.Engine = Engine

--- @enum (key) AUP.Engine.InteractionMode
local InteractionMode = {
  batchmode   = 'batchmode',
  nonstopmode = 'nonstopmode',
  scrollmode  = 'scrollmode',
  errorstopmode = 'errorstopmode'
}

Engine.InteractionMode = InteractionMode

function Engine.year()
  local p = Command.which('pdftex')
  if p then
    local m = string.match(p,'(%d%d%d%d)')
    if m then
      return m
    end
  end
end

--- @class AUP.K
--- @field engine 'engine'
--- @field engines 'engines'
--- @field exclude_engine 'exclude_engine'
--- @field exclude_engines 'exclude_engines'

K.engine = 'engine'
K.engines = 'engines'
K.exclude_engine = 'exclude_engine'
K.exclude_engines = 'exclude_engines'

Engine._tex_all = List()

Engine.no_lua_list = List{'pdftex', 'euptex', 'xetex'}

Engine.lua_list = List{'luatex', 'luahbtex', 'luajittex'}

Engine.list = List(Engine.no_lua_list):extend(Engine.lua_list)

do
  local l = Engine.list
  local iterator = AUP.arguments:iterator()
  local entry = iterator:next()
  while(entry) do
    if entry.key == K.engine then
      assert(entry:value_is_string(), "Expected: --%s=value (%s)"%{entry.key, type(entry.value)})
      if not l:contains(entry:string_value()) then
        l:append(entry:string_value())
      end
      entry:consume()
    elseif entry.key == K.engines then
      assert(entry:value_is_string(), "Expected: --%s=⟨cslist⟩"%{entry.key})
      l = pl_stringx.split(entry:string_value(), ',')
      entry:consume()
    elseif entry.key == K.exclude_engine then
      assert(entry:value_is_string(), "Expected: --%s=engine"%{entry.key})
      local ll = l:remove_value(((entry:string_value())))
      while ll ~= nil do
        ll = ll:remove_value(entry:string_value())
      end
      entry:consume()
    elseif entry.key == K.exclude_engines then
      assert(entry:value_is_string(), "Expected: --%s=⟨cslist⟩"%{entry.key})
      for x in pl_stringx.split(entry:string_value(), ','):iter() do
        local ll = l:remove_value(x)
        while ll ~= nil do
          ll = ll:remove_value(x)
        end
      end
      entry:consume()
    end
    entry = iterator:next()
  end
  Engine._tex_all = l
end

Engine._latex_all = List()

for prefix in pl_seq.list {'pdf', 'eup', 'xe', 'lua'} do
  if Engine._tex_all:contains(prefix..'tex') then
    Engine._latex_all:append(prefix..'latex')
  end
end

---An iterator over the tex engines
--- @return fun()
function Engine.tex_all()
  return Engine._tex_all:iter()
end

---An iterator over the latex engines
--- @return fun()
function Engine.latex_all()
  return Engine._latex_all:iter()
end

--- @class AUP.Engine
--- @field super fun(self: AUP.Engine, name: string)

local Dir = AUP.Dir

--- Initialize an Engine instance
--- @param name string
function Engine:_init(name)
  assert_string(2, name)
  print('DEBUG', 'name', name)
  local bin_p = Command.which(name)
  assert(bin_p, "Unknown engine "..name)
  if Command.dev and not name:find('latex') then
    local dev_p, d = Command.which(name)
    if d ~= nil then
      assert(dev_p, "Missing %s in %s"%{name, d})
    end
    if dev_p ~= nil then
      local touch_p = bin_p..".synctex_touch"
      local dev_mtime = lfs.attributes(dev_p, 'modification')
      local touch_mtime = lfs.attributes(touch_p, 'modification')
      if touch_mtime == nil or touch_mtime < dev_mtime then
        local saved_p = bin_p..".synctex_saved"
        if not pl_path.exists(saved_p) then
          assert(os.rename(bin_p, saved_p), "Unable to move %s to %s"%{bin_p, saved_p}, 2)
        end
        os.remove(bin_p)
        pl_dir.copyfile(dev_p, bin_p, true)
        --lfs.link (dev_p, bin_p, true)
        if pl_path.exists(touch_p) then
          lfs.touch(touch_p)
        else
          pl_utils.writefile(touch_p, '')
        end
        assert(pl_path.exists(touch_p))
        AUP.dbg:write(1, "Copy development engine: "..dev_p.."→"..bin_p)
      end
    else
      local saved_p = bin_p..".synctex_saved"
      if pl_path.exists(saved_p) then
        assert(os.remove(bin_p), 'Remove manually: '..bin_p, 2)
        assert(os.rename (saved_p, bin_p), "Unable to move a file", 2)
        AUP.dbg:write(1, "Reset development engine: "..saved_p.."→"..bin_p)
      end
    end
  end
  self:super(name)
end

--- Add a command line argument.
--- @param argument string
--- @return AUP.Engine
function Engine:add_argument(argument)
  assert_string (2, argument)
  self._arguments:append(argument)
  return self
end

--- Clear all the arguments.
--- @return AUP.Engine
function Engine:clear_arguments()
  self._arguments = List()
  return self
end

local quote_arg = pl_utils.quote_arg

--- Build the command on the fly.
--- @return string 
function Engine:cmd()
  assert(self._command, "Unknown command "..self._name)
  return quote_arg(List({
    self._command,
    self._synctex or false,
    self._interaction or false,
    self._file or false
  }):filter(function(x)
    return type(x)=='string' and #x>0
  end))
end

--- Set the `--synctex` option.
--- @param value string|number
--- @return AUP.Engine
function Engine:synctex(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  --assert_string (2, value)
  self._synctex = "--synctex="..value
  return self
end

--- Set the `--interaction` option.
--- @param value AUP.Engine.InteractionMode
--- @return AUP.Engine
function Engine:interaction(value)
  assert_string(2, value)
  self._interaction = "--interaction="..value
  return self
end

--- Set the file name to typeset.
--- @param value string
--- @return AUP.Engine
function Engine:file(value)
  assert_string(2, value)
  self._file = value
  return self
end

--- Reset the arguments.
--- @return AUP.Engine
function Engine:reset()
  self._synctex = nil
  self._interaction = nil
  self._file = nil
  return self
end

return {
  Engine = Engine,
}
