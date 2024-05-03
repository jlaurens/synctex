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
local PL = AUP.PL

local List = PL.List
local PL_utils = PL.utils
local assert_string = PL_utils.assert_string

local AUPCommand = AUP.module.Command

local dbg = AUP.dbg

--- @enum (key) AUPL3BuildTarget
local AUPL3BuildTarget = {
  Check   = 'check'
}

--- @class AUPL3Build: AUPCommand
--- @field _init fun(self: AUPL3Build, target: AUPL3BuildTarget)
--- @field reset fun(self: AUPL3Build)
--- @field synctex fun(self: AUPL3Build, value: integer): AUPL3Build
--- @field interaction fun(self: AUPL3Build, value: AUPTeXInteractionMode): AUPL3Build
--- @field file fun(self: AUPL3Build, file: string): AUPL3Build
--- @field cmd fun(self: AUPL3Build): string

local AUPL3Build = PL.class.AUPL3Build(AUPCommand)

--- Initialize an AUPL3Build instance
---@param target string
function AUPL3Build:_init(target)
  assert_string (2, target)
  self:super("l3build")
  self._target = target
end

--- Add an option.
--- @param argument string
--- @return AUPL3Build
function AUPL3Build:add_argument(argument)
  assert_string (2, argument)
  self._arguments:append(argument)
  return self
end

--- Add an option.
--- @return AUPL3Build
function AUPL3Build:clear_arguments()
  self._arguments = List()
  return self
end

local quote_arg = PL_utils.quote_arg

--- Build the command on the fly.
--- @return string 
function AUPL3Build:cmd()
  return quote_arg(List({
    self._command,
    self._target,
    self._config or false,
    self._date or false,
    self._debug or false,
    self._dirty or false,
    self._first or false,
    self._last or false,
    self._engine or false
  }):filter(function(x)
    return type(x)=='string' and #x>0
  end))
end

--- Set the `--config` option.
--- @param value string|number
--- @return AUPL3Build
function AUPL3Build:config(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  --assert_string (2, value)
  self._config = "--config="..value
  return self
end

--- Set the `--config` option.
--- @param value string|number
--- @return AUPL3Build
function AUPL3Build:date(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  --assert_string (2, value)
  self._date = "--date="..value
  return self
end

--- Set the `--debug` option.
--- @param value string|number
--- @return AUPL3Build
function AUPL3Build:debug(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  --assert_string (2, value)
  self._debug = "--debug="..value
  return self
end

--- Set the `--debug` option.
--- @param value string|number
--- @return AUPL3Build
function AUPL3Build:dirty(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  --assert_string (2, value)
  self._dirty = "--dirty="..value
  return self
end

--- Set the `--engine` option.
--- @param value string|number
--- @return AUPL3Build
function AUPL3Build:engine(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  --assert_string (2, value)
  self._command = "--engine="..value
  return self
end

--- Set the `--first` option.
--- @param value string|number
--- @return AUPL3Build
function AUPL3Build:first(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  --assert_string (2, value)
  self._first = "--first="..value
  return self
end

--- Set the `--last` option.
--- @param value string|number
--- @return AUPL3Build
function AUPL3Build:last(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  --assert_string (2, value)
  self._last = "--last="..value
  return self
end

--- Reset the arguments.
--- @return AUPL3Build
function AUPL3Build:reset()
  self._engine = nil
  self._date = nil
  self._first = nil
  self._last = nil
  self._debug = nil
  self._dirty = nil
  self._config = nil
  return self
end

--- @class AUP
--- @field L3Build AUPL3Build

AUP.L3Build = AUPL3Build

AUPL3Build.Target = AUPL3BuildTarget

return AUPL3Build
