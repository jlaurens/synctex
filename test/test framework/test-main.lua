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

local ipairs = ipairs
local match = string.match
local tonumber = tonumber

local __VERSION__ = '0.1'

print("Launching the SyncTeX testing framework...")

local synctex = {
  version = '0.1',
  description = 'Testing framework for SyncTeX'
}

-- make a readonly proxy available as module named 'synctex'
local function read_only_proxy(t)
  local proxy = {}
  local mt = {       -- create metatable
    __index = t,
    __newindex = function (_t,_k,_v)
      if t.__DESCRIPTION__ then
        error("attempt to update table "..t.__DESCRIPTION__, 2)
      else
        error("attempt to update table", 2)
      end
    end
  }
  setmetatable(proxy, mt)
  return proxy
end

synctex.version = '0.1'

local arguments = {}

directories = {}
units_by_directory = {}
local current_directory
local current_units = {}

-- parsing command line arguments
-- start with the --debug=⟨level⟩ for debugging level

synctex.debug_level = 0

local function do_something() end

for k,v in ipairs(arg) do
  k,v = match(v, "^%-%-([^=]+)=(.*)$")
  if k == 'debug' then
    synctex.debug_level = tonumber(v)
    do_something = function ()
      print("debug level:", synctex.debug_level)
    end
  end
end
do_something()

synctex.debug = function(level, ...)
  if level <= synctex.debug_level then
    local sep = ''
    local endl = ''
    for _,v in ipairs(...) do
      io.write(sep)
      sep = ','
      endl = '\n'
      io.write(v)
    end
    io.write(endl)
  end
end

-- parsing the arguments table arg into arguments:
for i,argument in ipairs(arg) do
  synctex.debug(1, "argument: "..i.."->"..argument)
  local k,v = match(argument, "^%-%-([^=]+)=(.*)$")
  if k == 'dir' then
    table.insert(directories, v)
    if current_directory then
      -- If current_directory is nil, keep the actual current_units table
      -- otherwise create a new one
      units_by_directory[current_directory] = read_only_proxy(current_units)
      current_units = {}
    end
    current_directory = v
    synctex.debug(1, "directory: ", v)
  elseif k == 'unit' then
    table.insert(current_units, v)
    synctex.debug(1, "directory: ", current_directory, "\nunit: ", v)
  elseif k == 'debug' then
  elseif k then
    -- another '--⟨key⟩=⟨value⟩' argument
    synctex.debug(1, k..": "..v)
    table.insert(arguments, read_only_proxy({
      key = k,
      value = v,
    }))
  else
    -- another '--⟨key⟩' argument
    k = match(argument, "^%-%-([^=]+)$")
    synctex.debug(1, "--"..argument)
    if k then
      table.insert(arguments, read_only_proxy({
        key = k,
        value = true,
      }))
      end
  end
end

if current_directory then
  units_by_directory[current_directory] = read_only_proxy(current_units)
  current_directory = nil
  current_units = nil
end

synctex.directories = read_only_proxy(directories)
synctex.units_by_directory = read_only_proxy(units_by_directory)

--[=====[!
Get all the argument values for the given key.
Returns an indexed table of the values in the order given.
--]=====]
synctex.get_all_arguments = function(key)
  assert(key)
  local ans = {}
  for _,v in ipairs(arguments) do
    if v.key == key then
      table.insert(ans, v.value)
    end
  end
  return ans
end
--[=====[!
Returns a deep copy of the indexed table of all the arguments.
Each item is a table with a ⟨key⟩ entry for key `key` and
a ⟨value⟩ entry for key value.
--]=====]
synctex.get_kv_arguments = function()
  local ans = {}
  for _,v in ipairs(arguments) do
    table.insert(ans,{key= v.key, value= v.value})
  end
  return ans
end
--[=====[!
Returns the last argument for the given key.
`nil` is returned if there is no such argument.
--]=====]
synctex.get_argument = function(key)
  local t = synctex.get_all_arguments(key)
  return t[#t]
end

-- now the package is setup, register it:
package.loaded['synctex'] = read_only_proxy(synctex)

-- Launch the tests from the parsed arguments
for _,k in ipairs(synctex.directories) do
  print('▶︎ Tests in directory: '..k)
  dofile(k..'/test.lua')
  local units = synctex.units_by_directory[k]
  for _,v in ipairs(units) do
    print('▬▬▶︎ Unit '..v..':')
  
  end
end

-- return the proxy
return require('synctex')
