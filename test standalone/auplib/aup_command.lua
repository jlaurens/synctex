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

local K = AUP.K

local pl_path   = require"pl.path"
local pl_class  = require"pl.class"
local List      = require"pl.List"
local pl_utils  = require"pl.utils"
local pl_pretty = require"pl.pretty"
local assert_string = pl_utils.assert_string
local executeex = pl_utils.executeex

local dbg = AUP.dbg
local TL = AUP.TL

--- @class AUP.Command: AUP.Class
local Command = pl_class()

AUP.Command = Command

--- The result of a comman d run
--- @class AUP.Command.Result: AUP.Class
--- @field commqnd AUP.Command
--- @field status boolean
--- @field code integer
--- @field stdout string
--- @field errout string
local Result = pl_class()

Command.Result = Result

--- Initialize an instance
--- @param command AUP.Command
--- @param status boolean
--- @param code integer
--- @param stdout string
--- @param errout string
function Result:_init(command, status, code, stdout, errout)
  pl_utils.assert_arg (3, status, 'boolean')
  pl_utils.assert_arg (4, code, 'number')
  pl_utils.assert_string(5, stdout)
  pl_utils.assert_string(6, errout)
  self.command = command
  self.status = status
  self.code = code
  self.stdout = stdout
  self.errout = errout
end

--- Build the command
--- @param msg string?
function Result:assert_success(msg)
  print('self.status', self.status)
  assert(self.status, msg or self.command:cmd())
end

--- @class AUP.Result.print.Arg
--- @field level integer?
--- @field ch string? defaults to '-'

local function print_arg(arg)
  if arg == nil then
    return {
      level = 0,
      ch = '-'
    }
  else
    pl_utils.assert_arg(1,arg,'table', nil, nil, 3)
  end
  return {
    level = arg.level or 0,
    ch = arg.ch or '-'
  }
end

--- Print the instance
---
--- @param arg AUP.Result.print.Arg?
function Result:print(arg)
  arg = print_arg(arg)
  if dbg:level_get()>9 then
    print('status: '..(self.status and 'true' or 'false'))
    print('code: '..(self.code ~= nil and self.code or 'nil'))
  end
  self:print_stdout(arg)
  self:print_errout(arg)
end

---- Print the instance
--- @param arg AUP.Result.print.Arg?
function Result:print_errout(arg)
  arg = print_arg(arg)
  if dbg:level_get()>(arg.level or 0) then
    if #self.errout>0 then
      AUP.br{label='errout', ch=arg.ch or '-'}
      print(self.errout)
      AUP.br{}
    else
      AUP.br{label='No errout', ch=arg.ch or '-'}
    end
  end
end

--- Print the instance
--- @param arg AUP.Result.print.Arg?
function Result:print_stdout(arg)
  arg = print_arg(arg)
  if dbg:level_get()>(arg.level or 0) then
    arg.label = 'stdout'
    AUP.br(arg)
    print(self.stdout)
  end
end

--- @class AUP.Command
--- @field os_concat string
--- @field os_setenv string

--- @diagnostic disable-next-line: undefined-field
if os.type == "windows" then
  Command.os_concat  = "&"
  Command.os_setenv  = "set"
else
  Command.os_concat  = ";"
  Command.os_setenv  = "export"
end

local arguments = AUP.arguments
assert(arguments, "Internal error")

--- @class AUP.Command
--- @field dev boolean

Command.dev = arguments.dev

--- @class AUP.K
--- @field PATHList 'PATHList'

K.PATHList = 'PATHList'

local storage = AUP.State.Compliant()

--- Set content to feed the `PATH` environment variable.
--- @param l pl.List -- list of strings
function Command.PATHList_set(l)
  assert(List:class_of(l))
  storage:state_set(K.PATHList, l)
end

--- Get content to feed the `PATH` environment variable.
--- @return pl.List
function Command.PATHList_get()
  local ans = storage:state_get(K.PATHList, true)
  if ans == nil then
    ans = List()
    Command.PATHList_set(ans)
    Command.PATHList_get = function ()
      return assert(storage:state_get_List(K.PATHList), 'A pl.List was expected')
    end
  end
  return ans
end

--- Prepend the given directory to the actual `PATH` list.
--- @param dir string -- must point to an existing directory
function Command.PATHList_promote(dir)
  assert_string(1, dir)
  assert(pl_path.isdir(dir))
  local l = Command.PATHList_get()
  l:remove_value(dir)
  l:insert(1, dir)
end

--- Get content to feed the `PATH` environment variable.
--- @return string
function Command.PATH_get()
  local l = Command.PATHList_get()
  return l:join(pl_path.dirsep)
end

--- @class AUP.K
--- @field ENV 'ENV'

K.ENV = 'ENV'

--- Set the CLI environement
---
--- This is not related to lua `_ENV` variable.
--- @param ... unknown key and value strings or table
function Command.set_ENV(...)
  local args = table.pack(...)
  local _ENV_Map = storage:state_get_Map(K.ENV)
  local i = 1
  while i<=args.n do
    local arg = args[i]
    if type(arg)=='table' then
      for k, v in pairs(arg) do
        if type(k)=='string' then
          if type(v)=='string' then
            _ENV_Map[k] = v
          else
            error('Only string value expected', 2)
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
        _ENV_Map[arg] = args[i]
        i = i + 1
      else
        error("Missing string value for key "..arg, 2)
      end
    else
      error('Only string or table expected', 2)
    end
  end
end

--- Class function similar to terminal's `which`.
---
--- If a file existsn at path `name` as is, `name` is returned normalized.
--- The `--Work` and `--bin_dir` options are used when provided.
--- The `PATH` environment variable content as well.
--- @param name string
--- @param dir string? 
--- @param only_dir boolean?
--- @return string? -- the full path to the engine, if any
--- @return string? -- if name is a relative paht, the directory containing name 
function Command.which(name, dir, only_dir)
  dbg:printf(9, [[
Command.which name:     %s
  dir:      %s
  only_dir: %s
]], name, dir or "none" , only_dir and "true" or "false")
  assert_string(1, name)
  if pl_path.is_windows and pl_path.extension(name)=="" then
    name = name .. '.exe'
  end
  if pl_path.isabs(name) then
    return pl_path.exists(name) and pl_path.normpath(name) or nil
  end
  if dir ~= nil then
    local full = pl_path.join(dir,name)
    dbg:write(9, "full: "..full)
    if pl_path.exists(full) then
      return pl_path.normpath(full), dir
    end
    if only_dir then
      return
    end
  end
  local l = Command.PATHList_get()
  if dbg:level_get()>9 then
    print("PATH:"..#l)
    l:foreach(function (x) print(x) end)
  end
  local res = l:map(function(x) return {n=pl_path.join(x,name), d=x} end)
  res = res:filter(function(x) return pl_path.exists(x.n) ~= false end)
  if #res > 0 then
    return pl_path.normpath(res[1].n), res[1].d
  end
  return AUP.Which.current_get():which(name)
end

-- --- Class function similar to terminal's `which` for development.
-- ---
-- --- If `name` exists, it is returned normalized.
-- --- The second value returned is the (path to) dev directory, if any.
-- --- @param name string
-- --- @return string?
-- --- @return string?
-- function Command.which_dev(name)
--   dbg:printf(9, [[
-- Command.which_dev name:     %s
-- ]], name)
--   assert_string(1, name)
--   if pl_path.is_windows and pl_path.extension(name)=="" then
--     name = name .. '.exe'
--   end
--   if pl_path.isabs(name) and pl_path.exists(name) then
--     return pl_path.normpath(name)
--   end
--   local p = storage:state_get_string(K.synctex_bin_dir)
--   if p ~= nil then
--     local full = pl_path.join(p,name)
--     if pl_path.exists(full) then
--       return pl_path.normpath(full), p
--     end
--   end
--   p = Command:bin_dir_get()
--   if p ~= nil then
--     local full = pl_path.join(p,name)
--     if pl_path.exists(full) then
--       return pl_path.normpath(full), p
--     end
--   end
-- end

--- Initialize an AUP.Command instance
---
--- `name` is a command name.
--- @param name string Required command name
--- @param command string? optional path to a command. When not provided,
--- `which` is used to find a command with the given name in the file system.
function Command:_init(name, command)
  assert_string(2, name)
  self._name = name
  if command ~= nil then
    assert_string(3, command)
    assert(pl_path.exists(command), "No file at "..command)
    self._command = pl_path.normpath(command)
  else
    self._command = Command.which(name)
    if self._command == nil then
      print("**** No command found for "..self._name)
    end
  end
end

--- Run the command
--- @param env table?
--- @return AUP.Command.Result
function Command:run(env)
  local L = List()
  local _ENV_Map = assert(storage:state_get_Map(K.ENV))
  L:append('%s %s="%s"' % {Command.os_setenv, "PATH", Command.PATH_get()})
  if env then
    for k,v in pairs(env) do
      if not string.find(k, " ") then
        L:append('%s %s="%s"' % {Command.os_setenv, k, v})
      end
    end
  end
  for k,v in _ENV_Map:iter() do
    if not string.find(k, " ") then
      if env == nil or env[k] == nil then
        L:append('%s %s="%s"' % {Command.os_setenv, k, v})
      end
    end
  end

  if dbg:level_get()>0 then
    L:append('echo "\\$PATH=$PATH"')
  end
  local short_cmd = self:cmd()
  L:append(short_cmd)
  local long_cmd = L:join(Command.os_concat)
  dbg:write(1,[[AUP.Command:run
  cwd: %s]]%{pl_path.currentdir()})
  if dbg:level_get()>1 then
    print('  cmd: `%s`'%{long_cmd})
    if dbg:level_get()>9 then
      long_cmd = 'echo "******** pwd=$(pwd)";'..long_cmd
    end
  else
    print('  cmd: `%s`'%{short_cmd})
  end
  local status, code, stdout, errout = executeex(long_cmd)
  if dbg:level_get() > 9 then
    pl_pretty(status, code, stdout, errout)
  end
  return Command.Result(self, status, code, stdout, errout)
end

--- Build the command
--- @return string
function Command:cmd()
  error("Virtual method", 2)
end

--- Reset the Command
--- @return string
function Command:reset()
  error("Virtual method", 2)
end

function Command:__tostring()
  return self:cmd()
end

-- --- local entry = arguments:get('Work')
-- List({'TEXMFSYSVAR', 'TEXMFCNF'}):map(
--   function(k)
--     local entry = arguments:get(k)
--     if entry ~= nil then
--       if entry:value_is_string() then
--         Command.set_ENV(k, entry.value)
--       end
--       entry:consume()
--     end
--   end
-- )

dbg:write(1, "aup_command loaded")

function Command.tex_bin_get()
  return TL.current_get():bin_dir_get()
end


return {
  Command = AUP.Command
}
