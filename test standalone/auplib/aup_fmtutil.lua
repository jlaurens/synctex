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

-- This is not yet used

--- @type AUP
local AUP = package.loaded.AUP

local dbg = AUP.dbg
local PL = AUP.PL

local PLList = PL.List
local PL_utils = PL.utils

local arguments = AUP.arguments
assert(arguments)

local AUPCommand = AUP.Command

--- @class AUPFmtUtil: AUPCommand
--- @field super fun(self: AUPFmtUtil, name: string)
--- @field _init fun(self: AUPFmtUtil)
--- @field reset fun(self: AUPFmtUtil): AUPFmtUtil
--- @field sys fun(self: AUPFmtUtil, yorn: boolean?): AUPFmtUtil
--- @field user fun(self: AUPFmtUtil, yorn: boolean?): AUPFmtUtil
--- @field n fun(self: AUPFmtUtil): AUPFmtUtil
--- @field dry_run fun(self: AUPFmtUtil): AUPFmtUtil
--- @field byengine fun(self: AUPFmtUtil, value: string): AUPFmtUtil
--- @field byfmt fun(self: AUPFmtUtil, value: string): AUPFmtUtil
--- @field cmd fun(self: AUPFmtUtil): string
--- @field texmf_var_dir fun(user: boolean?): string?
--- @field remaking fun(self: AUPFmtUtil): string?, string?
local AUPFmtUtil = PL.class.AUPFmtUtil(AUPCommand)

--- Initialize an AUPFmtUtil instance
function AUPFmtUtil:_init()
  self:super("fmtutil")
  self:sys()
end

local quote_arg = PL_utils.quote_arg

--- Build the command on the fly.
--- @return string 
function AUPFmtUtil:cmd()
  local list = PLList({
    self._command,
    self._sys or false,
    self._user or false,
    self._n or false,
    self._byfmt or false,
    self._byengine or false
  })
  list = list:filter(function(x)
    return type(x)=='string' and #x>0
  end)
  return quote_arg(list)
end

--- Set the `-n` option (dry run).
--- @return AUPFmtUtil
function AUPFmtUtil:n()
  self._n = "-n"
  return self
end

--- @return AUPFmtUtil
function AUPFmtUtil:dry_run()
  self._n = "-n"
  return self
end

--- Set the `--sys` option.
---
--- On by default
--- @param yorn boolean
--- @return AUPFmtUtil
function AUPFmtUtil:sys(yorn)
  if yorn==nil or yorn then
    self._sys = "--sys"
    self._user = nil
  else
    self:user(true)
  end
  return self
end

--- Set the `--user` option.
---@param yorn boolean
---@return AUPFmtUtil
function AUPFmtUtil:user(yorn)
  if yorn==nil or yorn then
    self._user = "--user"
    self._sys = nil
  else
    self:sys(true)
  end
  return self
end

--- Set the `--byengine` option.
--- @param value string
--- @return AUPFmtUtil
function AUPFmtUtil:byengine(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  self._byengine = "--byengine="..value
  self._byfmt = nil
  return self
end

--- Set the `--byfmt` option.
--- @param value string
--- @return AUPFmtUtil
function AUPFmtUtil:byfmt(value)
  if type(value) ~= 'string' then
    --- @diagnostic disable-next-line: cast-local-type
    value = assert(tostring(value))
  end
  self._byfmt = "--byfmt="..value
  self._byengine = nil
  return self
end

--- Reset the arguments.
--- @return AUPFmtUtil
function AUPFmtUtil:reset()
  self._n = nil
  self._byengine = nil
  self._byfmt = nil
  self:sys()
  return self
end

--- Format and engine to remake.
---
--- Runs fmtutil in a dry mode and parses the output.
--- @return string?
function AUPFmtUtil:remaking()
  local _n = self._n
  local result = self:n():run()
  self._n = _n
  if result.status then
    for l in PLList.split(result.stdout, "\n"):iter() do
      -- fmtutil [INFO]: --- remaking lualatex with luahbtex
      local fmt, engine = l:match("remaking%s+(%S+)%s+with%s+(%S+)$")
      if fmt ~= nil then
        return fmt, engine
      end
    end
  end
end

--- Remake a format when it is older than the engine.
function AUPFmtUtil:remake_when_older()
  local entry = arguments:get('no_remake')
  if entry and not entry:value_is_string() then
    return
  end
  local fmt, engine = self:remaking()
  assert(fmt, "Internal error(fmt)")
  assert(engine, "Internal error(engine)")
  local engine_p = AUPCommand.which(engine)
  local engine_mtime = PL.path.getmtime(engine_p)
  assert(engine_mtime)
  local var_dir = AUPFmtUtil.texmf_var_dir(self._sys == nil)
  local fmt_p = PL.path.join(var_dir, engine, fmt)..".fmt"
  local fmt_mtime = PL.path.getmtime(fmt_p)
  if fmt_mtime == nil or fmt_mtime < engine_mtime then
    local result = self:run()
    if dbg:level_get()>9 then
      print('AUPFmtUtil:remake_when_older')
      print("  engine_mtime", engine_p, engine_mtime)
      print("  fmt_mtime", fmt_p, fmt_mtime)
      if result then
        result:print()
      else
        print("  status: false")
      end
    end
  end
end

local _texmf_sys_var_dir
local _texmf_user_var_dir

--- The location where formats are stored.
---
--- Runs the `fmtutil` tool in dry mode and parses its output.
--- @param user boolean?
--- @return string?
function AUPFmtUtil.texmf_var_dir(user)
  if user then
    if _texmf_user_var_dir then
      return _texmf_user_var_dir
    end
  elseif _texmf_sys_var_dir then
    return _texmf_sys_var_dir
  end
  local fmtutil = AUPFmtUtil():user(user or false):n():byfmt("pdftex")
  local result = fmtutil:run()
  for l in PLList.split(result.stdout, "\n"):iter() do
    local ans = l:match("writing formats under%s*(.*)$")
    if ans ~= nil then
      if user then
        _texmf_user_var_dir = ans
      else
        _texmf_sys_var_dir = ans
      end
      return ans
    end
  end
end

--- @class AUP
--- @field FmtUtil AUPFmtUtil

AUP.FmtUtil = AUPFmtUtil

--- @enum AUPFormat
AUPFormat = {
  Plain = "plain",
  LaTeX = "latex",
  ConTeXt = "context",
}

return {
  FmtUtil = AUPFmtUtil,
  Format = AUPFormat,
}
