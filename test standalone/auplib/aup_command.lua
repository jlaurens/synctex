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

local AUP = package.loaded.AUP
local PL = AUP.PL
local PL_path = PL.path
local PL_class = PL.class

local PL_List = PL.List
local PL_utils = PL.utils
local assert_string = PL_utils.assert_string
local executeex = PL_utils.executeex

local dbg = AUP.dbg
local state = AUP.state

--- The result of a comman d run
--- @class AUPCommandResult
--- @field status boolean
--- @field code integer
--- @field stdout string
--- @field errout string
--- @field _init fun(self: AUPCommandResult, status: boolean, code: integer, stdout: string, errout: string)
--- @field print fun(self: AUPCommandResult, level: number|string?, ch: string?)
--- @field print_errout fun(self: AUPCommandResult, level: number|string?, ch: string?)
--- @field print_stdout fun(self: AUPCommandResult, level: number|string?, ch: string?)
local AUPCommandResult = PL_class()

--- Initialize an instance
--- @param status boolean
--- @param code integer
--- @param stdout string
--- @param errout string
function AUPCommandResult:_init(status, code, stdout, errout)
  PL.utils.assert_arg (2, status, 'boolean')
  PL.utils.assert_arg (3, code, 'number')
  PL.utils.assert_string(4, stdout)
  PL.utils.assert_string(5, errout)
  self.status = status
  self.code = code
  self.stdout = stdout
  self.errout = errout
end

--- Print the instance
---
--- @param level number|string? defaults to 0
--- @param ch string? defaults to '-'
function AUPCommandResult:print(level, ch)
  if type(level) == 'string' then
    ch = level
    level = 0
  end
  if dbg:level_get()>9 then
    print('status: '..(self.status and 'true' or 'false'))
    print('code: '..(self.code ~= nil and self.code or 'nil'))
  end
  self:print_stdout(level, ch)
  self:print_errout(level, ch)
end

--- Print the instance
--- @param level number|string? defaults to 0
--- @param ch string? defaults to '-'
function AUPCommandResult:print_errout(level, ch)
  if type(level) == 'string' then
    ch = level
    level = 0
  end
  if #self.errout>0 then
    AUP.br{label='errout', ch=ch or '-'}
    print(self.errout)
    AUP.br{}
  else
    AUP.br{label='No errout', ch=ch or '-'}
  end
end

--- Print the instance
--- @param level number|string? defaults to 0
--- @param ch string? defaults to '-'
function AUPCommandResult:print_stdout(level, ch)
  if type(level) == 'string' then
    ch = level
    level = 0
  end
  AUP.br{label='stdout',ch=ch or '-'}
  print(self.stdout)
end

local tdm = AUP.TeXDistMgr.singleton()

--- @class AUPCommand
--- @field Result AUPCommandResult
--- @field os_concat string
--- @field os_setenv string
--- @field set_ENV fun(...)
--- @field which fun(name: string, dir: string?, only_dir: boolean?): string?
--- @field which_dev fun(name: string): string?, string?
--- @field _init fun(self: AUPCommand, name: string, command: string?)
--- @field cmd fun(self: AUPCommand): string
--- @field reset fun(self: AUPCommand): AUPCommand
--- @field run fun(self: AUPCommand, env: table?): AUPCommandResult
local AUPCommand = PL.class()

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

--- @class AUPCommand
--- @field dev boolean

AUPCommand.dev = arguments.dev

--- @class AUPK
--- @field PATHList string

AUP.K.PATHList = 'PATHList'

--- @class AUPCommand
--- @field PATHList_get fun(): PL_List
--- @field PATH_get fun(): string

--- Get content to feed the `PATH` environment variable.
---@return PL_List
function AUPCommand.PATHList_get()
  local ans = state:get(AUP.K.PATHList, true)
  if ans == nil then
    ans = PL_List()
    for k in PL.seq.list {AUP.K.synctex_bin, AUP.K.tex_bin} do
      local d = state:get(k)
      if type(d) == 'string' then
        ans:append(d)
      elseif d ~= nil then
        error('Unexpected state: %s → %s'%{k, d}, 2)
      end
    end
    ans:extend(PL_List.split(os.getenv('PATH'), PL_path.dirsep))
    state:set(AUP.K.PATHList, ans)
  end
  return ans
end

--- Get content to feed the `PATH` environment variable.
---@return string
function AUPCommand.PATH_get()
  local l = AUPCommand.PATHList_get()
  return l:join(PL_path.dirsep)
end

--- @class AUPCommand
--- @field tex_bin_setup fun()
--- @field tex_bin_get fun(): string
--- @field tex_bin_set fun(dir: string|number?, dev: boolean?)

--- @class AUPK
--- @field tex_bin string

AUP.K.tex_bin = 'tex_bin'

--- Set the location where official reliable tex binaries are stored.
---
--- Used by `AUPCommand.tex_bin_setup()`
--- When the argument is not provided, it defaults to the result of
--- `latest_tex_bin` of the TeX distribution manager with the current year.
--- You can alse execute this command as part of some `test_setup_local_⟨cfg⟩.lua` file:
--- and invoke meson with
--- `meson test -C build --test-args='... --local=⟨cfg⟩ ...' ...`.
--- @param dir string|number? must point to an existing directory.
--- @param dev boolean? When true, does nothing if `AUPCommand.dev` is false.
function AUPCommand.tex_bin_set(dir, dev)
  if dev and not AUPCommand.dev then
    return
  end
  ---@type string|osdate
  if dir and (type(dir) == 'number' or string.match(dir, '^%d%d%d%d$')) then
    local y = tostring(dir)
    dir = nil
    if y then
      dir = tdm:latest_tex_bin(y)
    end
  else
    ---@diagnostic disable-next-line: assign-type-mismatch
    local y = os.date('!%Y')
    dir = tdm:latest_tex_bin(y)
  end
  PL.utils.assert_string(1, dir)
  state:set(AUP.K.tex_bin, dir)
  local p = AUPCommand.which("fmtutil", dir, true)
  if p == nil then
    print("WARNING: unavailable %s, "%{dir}, 2)
    for tex_bin in tdm:tex_bin_reversed_iter() do
      if tex_bin <= dir then
        p = AUPCommand.which("fmtutil", tex_bin, true)
        if p then
          print("WARNING: Using %s instead"%{tex_bin})
          state:set(AUP.K.tex_bin, tex_bin)
          return
        end
      end
    end
  else
    dbg:write(9, [[
AUPCommand.tex_bin_set:
  dir: %s
%s]]%{dir, debug.traceback(2)})
  end
end

--- The actual directory of official tex binaries.
---@return string
function AUPCommand.tex_bin_get()
  local ans = state:get(AUP.K.tex_bin)
  -- print('AUPCommand.tex_bin_get')
  -- print(debug.traceback())
  assert(ans, AUP.K.tex_bin.." not setup (see AUPCommand.tex_bin_set)")
  return ans
end


--- Sets the location where official reliable tex binaries are stored from arguments
---
--- Usage:
--- ```meson test -C build --test-args='... --tex_bin=<path/to/tex/binaries> ...' ...```
function AUPCommand.tex_bin_setup()
  local entry = arguments:get(AUP.K.tex_bin)
  if entry == nil then
    AUPCommand.tex_bin_set()
  elseif entry:value_is_string() then
    AUPCommand.tex_bin_set(entry:string_value())
    entry:consume()
  else
    error("`--%s` needs a value"%{AUP.K.tex_bin})
  end
  dbg:write(1,"AUPCommand.tex_bin_setup: "..AUPCommand.tex_bin_get())
end

--- @class AUPCommand
--- @field tex_dev_bin_get fun(): string
--- @field tex_dev_bin_set fun(dir)
--- @field tex_dev_bin_setup fun()

--- @class AUPK
--- @field tex_dev_bin string

AUP.K.tex_dev_bin = 'tex_dev_bin'

--- Sets the location where development tex binaries are stored.
---
--- @param dir string must point to an existing directory.
function AUPCommand.tex_dev_bin_set(dir)
  PL.utils.assert_string(1, dir)
  local p = AUPCommand.which("pdftex", dir, true)
  if p==nil then
    error("Unexpected %s"%{dir}, 2)
  else
    state:set(AUP.K.tex_dev_bin, dir)
  end
end

--- Sets the location where development tex binaries are stored from arguments
function AUPCommand.tex_dev_bin_setup()
  --- @type string
  local p
  local entry = arguments:get('Work')
  if entry ~= nil then
    if entry:value_is_string() then
      p = entry:string_value()
    else
      p = AUP.test_standalone_dir
      p = PL_path.join(p, "../../texlive-source/Work")
    end
    p = PL_path.join(p, "texk/web2c")
    p = PL_path.normpath(p)
    AUPCommand.tex_dev_bin_set(p)
    entry:consume()
    entry = arguments:get(AUP.K.tex_dev_bin)
    if entry ~= nil then
      entry:consume()
    end
  else
    entry = arguments:get(AUP.K.tex_dev_bin)
    if entry ~= nil then
      assert(entry:value_is_string(), "`--%s` needs a value"%{AUP.K.tex_dev_bin})
      AUPCommand.tex_dev_bin_set(entry:string_value())
      entry:consume()
    end
  end
  dbg:write(1,"AUPCommand.tex_dev_bin_setup"..AUPCommand.tex_dev_bin_get())
end

--- @class AUPCommand
--- @field synctex_bin_get fun(): string
--- @field synctex_bin_set fun(dir)
--- @field synctex_bin_setup fun()

--- @class AUPK
--- @field synctex_bin string

AUP.K.synctex_bin = 'synctex_bin'

--- Sets the location where synctex binaries are stored.
---
--- @param dir string must point to an existing directory.
function AUPCommand.synctex_bin_set(dir)
  PL.utils.assert_string(1, dir)
  state:setup()
  state:set(AUP.K.synctex_bin, dir)
  local p = AUPCommand.which("synctex", dir, true)
  state:teardown()
  if p==nil then
    error("Unexpected %s"%{dir}, 2)
  else
    state:set(AUP.K.synctex_bin, dir)
  end
end

--- Gets the location where synctex binaries are stored.
---@return string
function AUPCommand.synctex_bin_get()
  return state:get(AUP.K.synctex_bin)
end

--- Sets the location where development synctex binaries are stored from arguments
function AUPCommand.synctex_bin_setup()
  --- @type string
  local p
  local entry = arguments:get(AUP.K.synctex_bin)
  if entry ~= nil then
    assert(entry:value_is_string(), "`--%s` needs a value"%{AUP.K.synctex_bin})
    p = entry:string_value()
    entry:consume()
  else
    -- default
    p = PL_path.join(AUP.test_standalone_dir, '../meson/build')
  end
  AUPCommand.synctex_bin_set(PL_path.normpath(p))
  dbg:write(1,"AUPCommand.synctex_bin_setup: "..AUPCommand.synctex_bin_get())
end

--- @class AUPK
--- @field ENV string

AUP.K.ENV = 'ENV'

--- Set the CLI environement
---
--- This is not related to lua `_ENV` variable.
---@param ... unknown key and value strings or table
function AUPCommand.set_ENV(...)
  local args = table.pack(...)
  local _ENV_Map = state:mapGet(AUP.K.ENV)
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
--- If `name` exists, it is returned normalized.
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
]], name, dir or "none" , only_dir and "true" or "false")
  assert_string(1, name)
  if PL_path.is_windows and PL_path.extension(name)=="" then
    name = name .. '.exe'
  end
  if PL_path.isabs(name) and PL_path.exists(name) then
    return PL_path.normpath(name)
  end
  if dir ~= nil then
    local full = PL_path.join(dir,name)
    dbg:write(9, "full: "..full)
    if PL_path.exists(full) then
      return PL_path.normpath(full)
    end
    if only_dir then
      return
    end
  end
  local l = AUPCommand.PATHList_get()
  if dbg:level_get()>9 then
    print("PATH:"..#l)
    l:foreach(function (x) print(x) end)
  end
  local res = l:map(PL_path.join, name)
  res = res:filter(PL_path.exists)
  if #res > 0 then
    return PL_path.normpath(res[1])
  end
  return nil
end

--- Class function similar to terminal's `which` for development.
---
--- If `name` exists, it is returned normalized.
--- The second value returned is the (path to) dev directory, if any.
--- @param name string
--- @return string?
--- @return string?
function AUPCommand.which_dev(name)
  dbg:printf(9, [[
AUPCommand.which_dev name:     %s
]], name)
  assert_string(1, name)
  if PL_path.is_windows and PL_path.extension(name)=="" then
    name = name .. '.exe'
  end
  if PL_path.isabs(name) and PL_path.exists(name) then
    return PL_path.normpath(name)
  end
  local p = state:get(AUP.K.synctex_bin)
  if p ~= nil then
    local full = PL_path.join(p,name)
    if PL_path.exists(full) then
      return PL_path.normpath(full), p
    end
  end
  p = state:get(AUP.K.tex_dev_bin)
  if p ~= nil then
    local full = PL_path.join(p,name)
    if PL_path.exists(full) then
      return PL_path.normpath(full), p
    end
  end
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
    assert_string(3, command)
    assert(PL_path.exists(command), "No file at "..command)
    self._command = PL_path.normpath(command)
  else
    self._command = AUPCommand.which(name)
    if self._command == nil then
      print("**** No command found for "..self._name)
    end
  end
end

--- Run the command
--- @param env table?
--- @return boolean status
--- @return AUPCommandResult
function AUPCommand:run(env)
  local L = PL_List()
  local _ENV_Map = state:mapGet(AUP.K.ENV)
  L:append('%s %s="%s"' % {AUPCommand.os_setenv, "PATH", AUPCommand.PATH_get()})
  if env then
    for k,v in pairs(env) do
      if not string.find(k, " ") then
        L:append('%s %s="%s"' % {AUPCommand.os_setenv, k, v})
      end
    end
  end
  for k,v in _ENV_Map:iter() do
    if not string.find(k, " ") then
      if env == nil or env[k] == nil then
        L:append('%s %s="%s"' % {AUPCommand.os_setenv, k, v})
      end
    end
  end

  if dbg:level_get()>0 then
    L:append('echo "\\$PATH=$PATH"')
  end
  local short_cmd = self:cmd()
  L:append(short_cmd)
  local long_cmd = L:join(AUPCommand.os_concat)
  dbg:write(1,[[AUPCommand:run
  cwd: %s]]%{PL_path.currentdir()})
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
    PL.pretty(status, code, stdout, errout)
  end
  return AUPCommandResult(status, code, stdout, errout)
end

--- Build the command
--- @return string
function AUPCommand:cmd()
  error("Virtual method", 2)
end

--- Reset the Command
--- @return string
function AUPCommand:reset()
  error("Virtual method", 2)
end

function AUPCommand:__tostring()
  return self:cmd()
end

--- @class AUP
--- @field Command AUPCommand

AUP.Command = AUPCommand

-- --- local entry = arguments:get('Work')
-- PL_List({'TEXMFSYSVAR', 'TEXMFCNF'}):map(
--   function(k)
--     local entry = arguments:get(k)
--     if entry ~= nil then
--       if entry:value_is_string() then
--         AUPCommand.set_ENV(k, entry.value)
--       end
--       entry:consume()
--     end
--   end
-- )

dbg:write(1, "aup_command loaded")

return {
  Command = AUPCommand
}
