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

--- @class AUP
local AUP = package.loaded.AUP

local List     = require"pl.List"
local pl_utils = require"pl.utils"
local pl_class = require"pl.class"

local arguments = AUP.arguments
assert(arguments)

local Command = AUP.Command

--- @class AUP.KPSEWhich: AUP.Command
--- @field super fun(self: AUP.KPSEWhich, name: string)
local KPSEWhich = pl_class(Command)

--- Initialize an AUP.KPSEWhich instance
function KPSEWhich:_init()
  self:super("kpsewhich")
end

local quote_arg = pl_utils.quote_arg

--- Build the command on the fly.
--- @return string 
function KPSEWhich:cmd()
  local list = List({
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
--- @return AUP.KPSEWhich
function KPSEWhich:debug(N)
  self._debug = "-debug="..N
  return self
end

--- Set the `-all` option.
--- @return AUP.KPSEWhich
function KPSEWhich:all()
  self._all = "-all"
  return self
end

--- Set the filename
--- @param filename string
--- @return AUP.KPSEWhich
function KPSEWhich:filename(filename)
  self._filename = filename
  return self
end

--- Set the var name
--- @param var string
--- @return AUP.KPSEWhich
function KPSEWhich:var_value(var)
  self._var_value = "-var-value="..var
  return self
end

--- Set the var name
--- @param var string
--- @return AUP.KPSEWhich
function KPSEWhich:var_braced_value(var)
  self._var_braced_value = "-var-braced_value="..var
  return self
end

--- Set the engine
--- @param engine string
--- @return AUP.KPSEWhich
function KPSEWhich:engine(engine)
  self._engine = engine
  return self
end

--- Set the format
--- @param format string
--- @return AUP.KPSEWhich
function KPSEWhich:format(format)
  self._format = format
  return self
end

--- Set the progname
--- @param progname string
--- @return AUP.KPSEWhich
function KPSEWhich:progname(progname)
  self._progname = progname
  return self
end

--- Reset the arguments.
--- @return AUP.KPSEWhich
function KPSEWhich:reset()
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
function KPSEWhich.texmf_var_dir(user)
  if user then
    if _texmf_user_var_dir then
      return _texmf_user_var_dir
    end
  elseif _texmf_sys_var_dir then
    return _texmf_sys_var_dir
  end
  local KPSEWhich = AUP.KPSEWhich():user(user or false):n():byfmt("pdftex")
  local result = KPSEWhich:run()
  for l in List.split(result.stdout, "\n"):iter() do
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
--- @field KPSEWhich AUP.KPSEWhich

AUP.KPSEWhich = AUP.KPSEWhich

return {
  KPSEWhich = AUP.KPSEWhich
}
