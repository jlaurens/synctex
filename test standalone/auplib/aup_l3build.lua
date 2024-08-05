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

--- @class AUP
local AUP = package.loaded.AUP

local pl_class      = require"pl.class"
local List          = require"pl.List"
local pl_utils      = require"pl.utils"
local assert_string = pl_utils.assert_string

local Command = AUP.Command

--- @class AUP.L3Build: AUP.Command
--- @field super fun(self: AUP.L3Build, name: string)
local L3Build = pl_class(Command)

AUP.L3Build = L3Build

--- @enum (key) AUP.L3Build.Target
L3Build.Target = {
  Check   = 'check'
}

--- Initialize an AUP.L3Build instance
--- @param target string
function L3Build:_init(target)
  assert_string (2, target)
  self:super("l3build")
  self._target = target
end

--- Add an option.
--- @param argument string
--- @return AUP.L3Build
function L3Build:add_argument(argument)
  assert_string (2, argument)
  self._arguments:append(argument)
  return self
end

--- Add an option.
--- @return AUP.L3Build
function L3Build:clear_arguments()
  self._arguments = List()
  return self
end

local quote_arg = pl_utils.quote_arg

--- Build the command on the fly.
--- @return string 
function L3Build:cmd()
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
--- @return AUP.L3Build
function L3Build:config(value)
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
--- @return AUP.L3Build
function L3Build:date(value)
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
--- @return AUP.L3Build
function L3Build:debug(value)
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
--- @return AUP.L3Build
function L3Build:dirty(value)
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
--- @return AUP.L3Build
function L3Build:engine(value)
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
--- @return AUP.L3Build
function L3Build:first(value)
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
--- @return AUP.L3Build
function L3Build:last(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  --assert_string (2, value)
  self._last = "--last="..value
  return self
end

--- Reset the arguments.
--- @return AUP.L3Build
function L3Build:reset()
  self._engine = nil
  self._date = nil
  self._first = nil
  self._last = nil
  self._debug = nil
  self._dirty = nil
  self._config = nil
  return self
end

return {
  L3Build = L3Build
}
