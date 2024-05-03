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

--- @type AUP
local AUP = package.loaded.AUP
local lfs = package.loaded.lfs
local lfs = package.loaded.lfs
local PL = AUP.PL

local PLList = PL.List
local PL_utils = PL.utils
local assert_string = PL_utils.assert_string

local AUPCommand = AUP.Command

local state = AUP.state

--- @enum (key) AUPEngineInteractionMode
local AUPEngineInteractionMode = {
  batchmode   = 'batchmode',
  nonstopmode = 'nonstopmode',
  scrollmode  = 'scrollmode',
  errorstopmode = 'errorstopmode'
}

--- @class AUPEngine: AUPCommand
--- @field _init fun(self: AUPEngine, name: string)
--- @field reset fun(self: AUPEngine)
--- @field synctex fun(self: AUPEngine, value: integer): AUPEngine
--- @field interaction fun(self: AUPEngine, value: AUPEngineInteractionMode): AUPEngine
--- @field file fun(self: AUPEngine, file: string): AUPEngine
--- @field cmd fun(self: AUPEngine): string

local AUPEngine = PL.class.AUPEngine(AUPCommand)

---@class AUPK
---@field engine string
---@field engines string
---@field exclude_engine string
---@field exclude_engines string

AUP.K.engine = 'engine'
AUP.K.engines = 'engines'
AUP.K.exclude_engine = 'exclude_engine'
AUP.K.exclude_engines = 'exclude_engines'

AUPEngine._tex_all = PLList()

do
  local l = PLList({'pdftex', 'euptex', 'xetex', 'luatex', 'luahbtex', 'luajittex'})
  local iterator = AUP.arguments:iterator()
  local entry = iterator:next()
  while(entry) do
    if entry.key == AUP.K.engine then
      assert(entry:value_is_string(), "Expected: --%s=⟨cslist⟩"%{entry.key})
      if not l:contains((entry:string_value())) then
        l:append(entry:string_value())
      end
      entry:consume()
    elseif entry.key == AUP.K.engines then
      assert(entry:value_is_string(), "Expected: --%s=⟨cslist⟩"%{entry.key})
      l = PL.stringx.split(entry:string_value(), ',')
      entry:consume()
    elseif entry.key == AUP.K.exclude_engine then
      assert(entry:value_is_string(), "Expected: --%s=⟨cslist⟩"%{entry.key})
      local ll = l:remove_value(((entry:string_value())))
      while ll ~= nil do
        ll = ll:remove_value(entry:string_value())
      end
      entry:consume()
    elseif entry.key == AUP.K.exclude_engines then
      assert(entry:value_is_string(), "Expected: --%s=⟨cslist⟩"%{entry.key})
      for x in PL.stringx.split(entry:string_value(), ','):iter() do
        local ll = l:remove_value(x)
        while ll ~= nil do
          ll = ll:remove_value(x)
        end
      end
      entry:consume()
    end
    entry = iterator:next()
  end
  AUPEngine._tex_all = l
end

AUPEngine._latex_all = PLList()

for prefix in PL.seq.list {'pdf', 'eup', 'xe', 'lua'} do
  if AUPEngine._tex_all:contains(prefix..'tex') then
    AUPEngine._latex_all:append(prefix..'latex')
  end
end

---@class AUPEngine
---@field tex_all fun():fun()
---@field latex_all fun():fun()

---An iterator over the tex engines
---@return fun()
function AUPEngine.tex_all()
  return AUPEngine._tex_all:iter()
end

---An iterator over the tex engines
---@return fun()
function AUPEngine.latex_all()
  return AUPEngine._latex_all:iter()
end

--- Initialize an AUPEngine instance
--- @param name string
function AUPEngine:_init(name)
  assert_string(2, name)
  local bin_p = AUPCommand.which(name, AUPCommand.tex_bin_get(), true)
  assert(bin_p, "Unknown engine "..name)
  if AUPCommand.dev and not name:find('latex') then
    local dev_p, d = AUPCommand.which_dev(name)
    if d ~= nil then
      assert(dev_p, "Missing %s in %s"%{name, d})
    end
    if dev_p ~= nil then
      local touch_p = bin_p..".synctex_touch"
      local dev_mtime = lfs.attributes(dev_p, 'modification')
      local touch_mtime = lfs.attributes(touch_p, 'modification')
      if touch_mtime == nil or touch_mtime < dev_mtime then
        local saved_p = bin_p..".synctex_saved"
        if not PL.path.exists(saved_p) then
          assert(os.rename(bin_p, saved_p), "Unable to move %s to %s"%{bin_p, saved_p}, 2)
        end
        os.remove(bin_p)
        PL.dir.copyfile(dev_p, bin_p, true)
        --lfs.link (dev_p, bin_p, true)
        if PL.path.exists(touch_p) then
          lfs.touch(touch_p)
        else
          PL.utils.writefile(touch_p, '')
        end
        assert(PL.path.exists(touch_p))
        AUP.dbg:write(1, "Copy development engine: "..dev_p.."→"..bin_p)
      end
    else
      local saved_p = bin_p..".synctex_saved"
      if PL.path.exists(saved_p) then
        assert(os.remove(bin_p), 'Remove manually: '..bin_p, 2)
        assert(os.rename (saved_p, bin_p), "Unable to move a file", 2)
        AUP.dbg:write(1, "Reset development engine: "..dev_p.."→"..bin_p)
      end
    end
  end
  self:super(name)
end

--- Add an option.
--- @param argument string
--- @return AUPEngine
function AUPEngine:add_argument(argument)
  assert_string (2, argument)
  self._arguments:append(argument)
  return self
end

--- Add an option.
--- @return AUPEngine
function AUPEngine:clear_arguments()
  self._arguments = PLList()
  return self
end

local quote_arg = PL_utils.quote_arg

--- Build the command on the fly.
--- @return string 
function AUPEngine:cmd()
  assert(self._command, "Unknown command "..self._name)
  return quote_arg(PLList({
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
--- @return AUPEngine
function AUPEngine:synctex(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  --assert_string (2, value)
  self._synctex = "--synctex="..value
  return self
end

--- Set the `--interaction` option.
--- @param value AUPEngineInteractionMode
--- @return AUPEngine
function AUPEngine:interaction(value)
  assert_string(2, value)
  self._interaction = "--interaction="..value
  return self
end

--- Set the file name to typeset.
--- @param value string
--- @return AUPEngine
function AUPEngine:file(value)
  assert_string(2, value)
  self._file = value
  return self
end

--- Reset the arguments.
--- @return AUPEngine
function AUPEngine:reset()
  self._synctex = nil
  self._interaction = nil
  self._file = nil
  return self
end

--- @class AUP
--- @field Engine AUPEngine

AUP.Engine = AUPEngine

AUPEngine.InteractionMode = AUPEngineInteractionMode

return {
  Engine = AUPEngine,
  InteractionMode = AUPEngineInteractionMode
}
