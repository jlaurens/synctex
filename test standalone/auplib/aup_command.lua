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
local executeex = PL_utils.executeex

local dbg = AUP.dbg

--- @class AUPCommand
--- @field set_ENV fun(...)
--- @field PATH_list fun(): List
--- @field PATH fun(): string
--- @field which fun(name: string, dir: string?, only_dir: boolean?): string?
--- @field _init fun(self: AUPCommand, name: string, command: string?)
--- @field cmd fun(self: AUPCommand): string
--- @field reset fun(self: AUPCommand): AUPCommand
--- @field os_concat string
--- @field os_setenv string
local AUPCommand = PL.class.AUPCommand()

---@diagnostic disable-next-line: undefined-field
if os.type == "windows" then
  AUPCommand.os_concat  = "&"
  AUPCommand.os_setenv  = "set"
else
  AUPCommand.os_concat  = ";"
  AUPCommand.os_setenv  = "export"
end

local arguments = AUP.arguments
assert(arguments, "Internal error")

local _PATH_list = nil
local _ENV_table = {}
local _tex_bin_dir = nil
local _tex_dev_bin_dir = nil

--- Set the environement
---@param ... unknown key and value strings or table
function AUPCommand.set_ENV(...)
  local args = table.pack(...)
  local i = 1
  while i<=args.n do
    local arg = args[i]
    if type(arg)=='table' then
      for k, v in pairs(arg) do
        if type(k)=='string' then
          if type(v)=='string' then
            _ENV_table[k] = v
          else
            error('Only string key expected', 2)
          end
        else
          error('Only string key expected', 2)
        end
      end
      i = i + 1
    elseif type(arg)=='string' then
      i = i + 1
      if i>args.n then
        error("Missing value for key "..arg, 2)
      end
      if type(args[i])=='string' then
        _ENV_table[arg] = args[i]
        i = i + 1
      else
        error("Missing string value for key "..arg, 2)
      end
    else
      error('Only string or table expected', 2)
    end
  end
end

function AUPCommand.PATH_list()
  AUPCommand.setup_tex_bin_dir()
  AUPCommand.setup_tex_dev_bin_dir()
  if _PATH_list == nil then
    _PATH_list = List()
    if _tex_dev_bin_dir ~= nil then
      _PATH_list:append(_tex_dev_bin_dir)
    end
    if _tex_bin_dir ~= nil then
      _PATH_list:append(_tex_bin_dir)
    end
    _PATH_list:extend(List.split(os.getenv('PATH'), PL.path.dirsep))
  end
  return _PATH_list
end

---The list of PATH components
---@return string
function AUPCommand.PATH()
  return AUPCommand.PATH_list():join(PL.path.dirsep)
end

--- Class function similar to terminal's `which`.
---
--- If `name` exits, it is returned normalized.
--- The `--Work` and `--bin_dir` options are used if provided.
--- The `PATH` environment variable content as well.
--- @param name string
--- @param dir string?
--- @param only_dir boolean?
--- @return string?
function AUPCommand.which(name, dir, only_dir)
  dbg:printf(9, [[
AUPCommand.which name:     %s
                 dir:      %s
                 only_dir: %s
]], name, dir or "None" , only_dir and "True" or "False")
  assert_string(2, name)
  if PL.path.is_windows and PL.path.extension(name)=="" then
    name = name .. '.exe'
  end
  if PL.path.isabs(name) and PL.path.exists(name) then
    return PL.path.normpath(name)
  end
  if dir ~= nil then
    name = PL.path.join(dir,name)
    if PL.path.exists(name) then
      return PL.path.normpath(name)
    end
    if only_dir then
      return
    end
  end
  local l = AUPCommand.PATH_list()
  if dbg:level_get()>9 then
    print("PATH:")
    l:foreach(function (x) print(x) end)
  end
  local res = l:map(PL.path.join, name)
  res = res:filter(PL.path.exists)
  if #res > 0 then
    return PL.path.normpath(res[1])
  end
  return nil
end

--- Initialize an AUPCommand instance
---
--- `name` is a command name.
--- @param name string Required command name
--- @param command string? optional path to a command. When not provided,
--- `which` is used to find a command with the given name in the file system.
function AUPCommand:_init(name, command)
  assert_string(2, name)
  self._name = name
  if command ~= nil then
    assert_string(2, command)
    assert(PL.path.exists(command), "No file at "..command)
    self._command = PL.path.normpath(command)
  else
    self._command = AUPCommand.which(name)
    if self._command == nil then
      print("**** No command found for "..self._name)
      print('PATH: '..os.getenv('PATH'))
    end
  end
end

--- Run the command
--- @param env table?
--- @return boolean status
--- @return integer code
--- @return string stdout
--- @return string errout
function AUPCommand:run(env)
  local L = List()
  ---@diagnostic disable-next-line: undefined-field
  if env == nil or (env.PATH == nil and _ENV_table.PATH == nil) then
    L:append('%s %s="%s"' % {AUPCommand.os_setenv, "PATH", AUPCommand.PATH()})
  end
  if env then
    for k,v in pairs(env) do
      if not string.find(k, " ") then
        L:append('%s %s="%s"' % {AUPCommand.os_setenv, k, v})
      end
    end
  end
  for k,v in pairs(_ENV_table) do
    if not string.find(k, " ") then
      if env == nil or env[k] == nil then
        L:append('%s %s="%s"' % {AUPCommand.os_setenv, k, v})
      end
    end
  end

  local short_cmd = self:cmd()
  L:append(short_cmd)
  local long_cmd = L:join(AUPCommand.os_concat)
  print('cwd: '..PL.path.currentdir())
  if dbg:level_get()>1 then
    print('cmd: `'..long_cmd..'`')
  else
    print('cmd: `'..short_cmd..'`')
  end
  return executeex(long_cmd)
end

--- Build the command
--- @return string
function AUPCommand:cmd()
  error("Virtual method", 2)
end

--- Reset the arguments
--- @return string
function AUPCommand:reset()
  error("Virtual method", 2)
end

--- Sets the location where official reliable tex binaries are stored.
---
--- @param dir string must point to an existing directory.
function AUPCommand.set_tex_bin_dir(dir)
  PL.utils.assert_string(1, dir)
  _tex_bin_dir = dir
  _PATH_list = nil
  local fmtutil = AUPCommand.which("fmtutil", dir)
  assert(fmtutil~=nil,"Unexpected %s"%{dir})
end

--- Sets the location where development tex binaries are stored.
---
--- @param dir string must point to an existing directory.
function AUPCommand.set_tex_dev_bin_dir(dir)
  PL.utils.assert_string(1, dir)
  _tex_dev_bin_dir = dir
  _PATH_list = nil
  local pdftex_p = AUPCommand.which("pdftex", dir, true)
  assert(pdftex_p~=nil,"Unexpected %s"%{dir})
end

--- Sets the location where development tex binaries are stored from arguments
---
--- One shot function
function AUPCommand.setup_tex_dev_bin_dir()
  local p
  local entry = arguments:get('Work')
  if entry ~= nil then
    if entry:value_is_string() then
      p = entry.value
    else
      p = AUP.test_standalone_dir
      p = PL.path.join(p, "../../texlive-source/Work")
    end
    p = PL.path.join(p, "texk/web2c/")
    AUPCommand.set_tex_dev_bin_dir(p)
    entry:consume()
    entry = arguments:get('dev_bin_dir')
    if entry ~= nil then
      entry:consume()
    end
  else
    entry = arguments:get('dev_bin_dir')
    if entry ~= nil then
      if entry:value_is_string() then
        ---@diagnostic disable-next-line: param-type-mismatch
        AUPCommand.set_tex_dev_bin_dir(entry.value)
        entry:consume()
      else
        error("`dev_bin_dir` needs a value")
      end
    end
  end
  AUPCommand.setup_tex_dev_bin_dir = function() end
end

--- Sets the location where official reliable tex binaries are stored from arguments
---
--- One shot function
function AUPCommand.setup_tex_bin_dir()
  local entry = arguments:get('bin_dir')
  if entry ~= nil then
    if entry:value_is_string() then
      ---@diagnostic disable-next-line: param-type-mismatch
      AUPCommand.set_tex_bin_dir(entry.value)
      entry:consume()
    else
      error("`bin_dir` needs a value")
    end
  end
  AUPCommand.setup_tex_bin_dir = function() end
end

function AUPCommand:__tostring()
  return self:cmd()
end

AUP.Command = AUPCommand

return AUPCommand
