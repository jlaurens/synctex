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
local PL_path = PL.path
local PL_utils = PL.utils
local assert_string = PL_utils.assert_string
local executeex = PL_utils.executeex

--- @class AUPCommand
--- @field _init fun(self: AUPCommand, name: string)
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

--- Initialize an AUPEngine instance
--- @param name string
function AUPCommand:_init(name)
  assert_string(2, name)
  self._name = name
  if PL_path.is_windows and not PL_path.extension(name) then
    self._name = name + '.exe'
  end
  self.PATH = List(self.PATH)
  local res = self.PATH:map(PL_path.join, self._name)
  res = res:filter(PL_path.exists)
  if #res > 0 then
    self._engine = res[1]
    return
  end
  if not PATH then
    PATH = List()
    if arguments then
      local iterator = arguments:iterator()
      local entry = iterator:next()
      while(entry) do
        if entry.key == 'bin_dir' then
          if  type(entry.value) == 'string' then
            PATH:append(entry.value)
          end
          iterator:consume()
        end
        entry = iterator:next()
      end
    end
    PATH:extend(List.split(os.getenv('PATH'), PL_path.dirsep))
  end
  res = PATH:map(PL_path.join, self._name)
  res = res:filter(PL_path.exists)
  if #res > 0 then
    self._engine = res[1]
    return
  end
  print("**** No engine found for "..self._name)
  print('PATH: '..os.getenv('PATH'))
end

--- Run the command
--- @param env table?
--- @return boolean status
--- @return integer code
--- @return string stdout
--- @return string errout
function AUPCommand:run(env)
  local L = List()
  if env then
    for k,v in pairs(env) do
      if not string.find(k, " ") then
        L:append('%s %s="%s"' % {AUPCommand.os_setenv, k, v})
      end
    end
  end
  L:append(self:cmd())
  local cmd = L:join(AUPCommand.os_concat)
  print('cmd: `'..cmd..'`')
  return executeex(cmd)
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

AUP.Command = AUPCommand

return AUPCommand
