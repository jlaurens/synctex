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

--[==[
This script is executed by some `texlua` binary.
This binary was launched in a situation that we do not expect to control.
For that reason, we cannot use the `kpse` table because it is configured
to use a TeX distribution that is not well defined.
We launch `KPSEWhich` by hand.
--]==]

--- @type AUP
local AUP = package.loaded.AUP

local dbg = AUP.dbg
local PL = AUP.PL

local PL_class = PL.class
local PL_List = PL.List
local PL_utils = PL.utils

local arguments = AUP.arguments
assert(arguments)

local AUPCommand = AUP.Command

--- @class AUPKPSEWhich: AUPCommand
--- @field super fun(self: AUPKPSEWhich, name: string)
--- @field _init fun(self: AUPKPSEWhich)
--- @field reset fun(self: AUPKPSEWhich): AUPKPSEWhich
--- @field all fun(self: AUPKPSEWhich): AUPKPSEWhich
--- @field engine fun(self: AUPKPSEWhich, engine: string): AUPKPSEWhich
--- @field format fun(self: AUPKPSEWhich, format: string): AUPKPSEWhich
--- @field progname fun(self: AUPKPSEWhich, progname: string): AUPKPSEWhich
--- @field debug fun(self: AUPKPSEWhich, N: string|number): AUPKPSEWhich
--- @field filename fun(self: AUPKPSEWhich, filename: string): AUPKPSEWhich
--- @field var_value fun(self: AUPKPSEWhich, var: string): AUPKPSEWhich
--- @field var_braced_value fun(self: AUPKPSEWhich, var: string): AUPKPSEWhich
local AUPKPSEWhich = PL_class(AUPCommand)

--- Initialize an AUPKPSEWhich instance
function AUPKPSEWhich:_init()
  self:super("kpsewhich")
end

local quote_arg = PL_utils.quote_arg

--- Build the command on the fly.
--- @return string 
function AUPKPSEWhich:cmd()
  local list = PL_List({
    self._command,
    self._all or false,
    self._engine or false,
    self._format or false,
    self._progname or false,
    self._debug or false,
    self._filename or false,
    self._var_value or false,
    self._var_braced_value or false,
  })
  list = list:filter(function(x)
    return type(x)=='string' and #x>0
  end)
  return quote_arg(list)
end

--- Set the `-debug` option.
--- @param N string|number
--- @return AUPKPSEWhich
function AUPKPSEWhich:debug(N)
  self._debug = "-debug="..N
  return self
end

--- Set the `-all` option.
--- @return AUPKPSEWhich
function AUPKPSEWhich:all()
  self._all = "-all"
  return self
end

--- Set the filename
--- @param filename string
--- @return AUPKPSEWhich
function AUPKPSEWhich:filename(filename)
  self._filename = filename
  return self
end

--- Set the var name
--- @param var string
--- @return AUPKPSEWhich
function AUPKPSEWhich:var_value(var)
  self._var_value = "-var-value="..var
  return self
end

--- Set the var name
--- @param var string
--- @return AUPKPSEWhich
function AUPKPSEWhich:var_braced_value(var)
  self._var_braced_value = "-var-braced_value="..var
  return self
end

--- Set the engine
--- @param engine string
--- @return AUPKPSEWhich
function AUPKPSEWhich:engine(engine)
  self._engine = engine
  return self
end

--- Set the format
--- @param format string
--- @return AUPKPSEWhich
function AUPKPSEWhich:format(format)
  self._format = format
  return self
end

--- Set the progname
--- @param progname string
--- @return AUPKPSEWhich
function AUPKPSEWhich:progname(progname)
  self._progname = progname
  return self
end

--- Reset the arguments.
--- @return AUPKPSEWhich
function AUPKPSEWhich:reset()
  self._all = nil
  self._engine = nil
  self._format = nil
  self._progname = nil
  self._debug = nil
  self._filename = nil
  self._var_value = nil
  self._var_braced_value = nil
  return self
end


--- The location where formats are stored.
---
--- Runs the `KPSEWhich` tool in dry mode and parses its output.
--- @param user boolean?
--- @return string?
function AUPKPSEWhich.texmf_var_dir(user)
  if user then
    if _texmf_user_var_dir then
      return _texmf_user_var_dir
    end
  elseif _texmf_sys_var_dir then
    return _texmf_sys_var_dir
  end
  local KPSEWhich = AUPKPSEWhich():user(user or false):n():byfmt("pdftex")
  local result = KPSEWhich:run()
  for l in PL_List.split(result.stdout, "\n"):iter() do
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
--- @field KPSEWhich AUPKPSEWhich

AUP.KPSEWhich = AUPKPSEWhich

return {
  KPSEWhich = AUPKPSEWhich
}
