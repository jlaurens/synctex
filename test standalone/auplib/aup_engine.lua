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
local PL = AUP.PL

local List = PL.List
local PL_utils = PL.utils
local assert_string = PL_utils.assert_string

local AUPCommand = AUP.module.command

--- @enum (key) AUPTeXInteractionMode
local AUPTeXInteractionMode = {
  batchmode   = 'batchmode',
  nonstopmode = 'nonstopmode',
  scrollmode  = 'scrollmode',
  errorstopmode = 'errorstopmode'
}

--- @class AUPEngine: AUPCommand
--- @field _init fun(self: AUPEngine, name: string)
--- @field reset fun(self: AUPEngine)
--- @field synctex fun(self: AUPEngine, value: integer): AUPEngine
--- @field interaction fun(self: AUPEngine, value: AUPTeXInteractionMode): AUPEngine
--- @field file fun(self: AUPEngine, file: string): AUPEngine
--- @field cmd fun(self: AUPEngine): string

local AUPEngine = PL.class.AUPEngine(AUPCommand)

--- Initialize anAUPEngine instance
--- @param name string
function AUPEngine:_init(name)
  assert_string(2, name)
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
  self._arguments = List()
  return self
end

local quote_arg = PL_utils.quote_arg

--- Build the command on the fly.
--- @return string 
function AUPEngine:cmd()
  return quote_arg(List({
    self._engine,
    self._synctex,
    self._interaction,
    self._file
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
--- @param value AUPTeXInteractionMode
--- @return AUPEngine
function AUPEngine:interaction(value)
  assert_string(2, value)
  self._interaction = "--interaction="..value
  return self
end

--- Set the `--interaction` option.
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

AUP.Engine = AUPEngine

AUPEngine.InteractionMode = AUPTeXInteractionMode

return AUPEngine
