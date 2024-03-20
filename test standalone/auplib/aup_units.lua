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

local pushd = AUP.pushd
local popd  = AUP.popd

local dbg = AUP.dbg

local PL = AUP.PL

--- @alias StringsByString { string: string[] }

--- @class AUPUnits
--- @field _engine_suites string[]
--- @field _library_suites string[]
--- @field _test_suites { engine: string[], library: string[] }
--- @field _units_by_suite { engine: StringsByString, library: StringsByString }
--- @field _engine { test_suites: string[], units_by_suite: StringsByString }
--- @field _library { test_suites: string[], units_by_suite: StringsByString }
--- @field check fun(self: AUPUnits)
--- @field check_suite fun(self: AUPUnits, dir: string?, units: string[]?)
--- @field check_unit fun(self: AUPUnits, unit: string)
--- @field load fun(self: unknown, name: string), class methods
--- @field test_setup fun(self: unknown)
--- @field test fun(self: unknown)
--- @field test_teardown fun(self: unknown)
--- @field test_currentdir fun(self: unknown, exclude: table?)

local AUPUnits = PL.class.AUPUnits()

--- nitialize an `AUPUnits` instance
--- @param arguments AUPArguments
function AUPUnits:_init(arguments)
  --self:super()   -- call the ancestor initializer if needed
  local engine_suites = {}
  local library_suites = {}
  local units_by_engine_suite = {}
  local units_by_library_suite = {}
  local current_suite = nil
  local current_units = {}
  local current_suites = library_suites
  local units_by_current_suite = units_by_library_suite
  local iterator = arguments:iterator()
  local entry = iterator:next()
  dbg:write(1, "**** Managing arguments")
  while(entry) do
    if entry.key == 'suite' then
      table.insert(current_suites, entry.value)
      if current_suite then
        -- If current_suite is nil, keep the actual current_units table
        -- otherwise create a new one
        units_by_current_suite[current_suite] = current_units
        current_units = {}
      end
      current_suite = entry.value
      dbg:printf(1, "suite: %s\n", current_suite)
      iterator:consume()
    elseif entry.key == 'unit' then
      table.insert(current_units, entry.value)
      dbg:printf(1, "suite: %s, unit: %s\n", current_suite, entry.value)
      iterator:consume()
    elseif entry.key == 'engine' then
      if current_suites ~= engine_suites then
        dbg:write(1, "mode: engine")
        if current_suite then
          units_by_current_suite[current_suite] = current_units
        end
        current_suites = engine_suites
        units_by_current_suite = units_by_engine_suite
        -- no reasonable default value for current_suite
        current_suite = nil
        current_units = {}
      end
      iterator:consume()
    elseif entry.key == 'library' then
      if current_suites ~= library_suites then
        dbg:write(1, "mode: library")
        if current_suite then
          units_by_current_suite[current_suite] = current_units
        end
        current_suites = library_suites
        units_by_current_suite = units_by_library_suite
        -- no reasonable default value for current_suite
        current_suite = nil
        current_units = {}
      end
      iterator:consume()
    end
    entry = iterator:next()
  end
  if current_suite then
    units_by_current_suite[current_suite] = current_units
    current_suite = nil
    current_units = nil
  end
  self._test_suites = {
    engine = engine_suites,
    library = library_suites,
  }
  self._units_by_suite = {
    engine = units_by_engine_suite,
    library = units_by_library_suite,
  }
  self._engine = {
    test_suites = self._test_suites.engine,
    units_by_suite = self._units_by_suite.engine,
  }
  self._library = {
    test_suites = self._test_suites.library,
    units_by_suite = self._units_by_suite.library,
  }
  dbg:printf(10, "%s\n", self)
  dbg:write(10, self)
end


function AUPUnits:__tostring()
  local ans = PL.pretty.write (self)
  return 'AUPUnits: '..ans
end

local PL_path = PL.path
local currentdir = PL_path.currentdir

--- oad a file in the current directory.
--- lass method.
--- @param self unknown
--- @param name string
function AUPUnits:load(name)
  local f = loadfile(name..'.lua')
  if f then
    dbg:write(1, "Loading "..name..'.lua')
    f()
  else
    dbg:write(1, "No "..name..'.lua')
  end
end

--- oad the `test_setup.lua` of the current directory, if any.
function AUPUnits:test_setup()
  print('Test setup for '..AUP:short_path())
  self:load('test_setup')
end

--- oad the `test.lua` of the current directory, if any.
function AUPUnits:test()
  print('Test for '..AUP:short_path())
  self:load('test')
end

--- oad the `test_teardown.lua` of the current directory, if any.
function AUPUnits:test_teardown()
  print('Test teardown for '..AUP:short_path())
  self:load('test_teardown')
end

--- ake all the tests
--- @param self AUPUnits
function AUPUnits:check()
  self:test_setup()
  for _,key in ipairs({'_library', '_engine'}) do
    if pushd('test'.. key ..'/') then
      self:test_setup()
      local mode = self[key]
      if mode.test_suites then
        for _,suite in ipairs(mode.test_suites) do
          self:check_suite(suite, mode.units_by_suite[suite])
        end--for
        self:test_teardown()
      else
        self:test()
      end--mode.test_suites
      popd()
    end
  end-- for '_library', '_engine'
  self:test_teardown()
end

--- ake all the tests in the given directory
--- @param self AUPUnits
--- @param suite string|string[]?
--- @param units string[]?
function AUPUnits:check_suite(suite, units)
  if suite then
    if type(suite) == "table" then
      units = suite
      suite = "."
    end
  else
    suite = "."
  end
  if pushd(suite) then
    print('▶︎ Tests in directory: '..currentdir())
    self:test_setup()
    if units and #units > 0 then
      for _,unit in ipairs(units) do
        self:check_unit(unit)
      end
    else--if not units then
      self:test()
    end
    self:test_teardown()
    popd()
  else
    print('▶︎ No test for suite: "'..suite..'".')
  end
end

--- ake all the tests for the given unit.
--- unit` is a folder in the current working directory.
--- @param self AUPUnits
--- @param unit string
function AUPUnits:check_unit(unit)
  if pushd(unit) then
    print('▬▬▶︎ Unit '..unit..':')
    self:test()
    popd()
  else
    print('▬▬▶︎ No test for unit "'..unit..'".')
  end
end

local getdirectories = PL.dir.getdirectories
local basename = PL_path.basename
local List = PL.List

--- ake tests in the current working directory.
--- @param exclude table?
function AUPUnits:test_currentdir(exclude)
  local dirs = getdirectories()
  for _,p in ipairs(dirs) do
    local bn = basename(p)
    local list = List(exclude)
    if not list:contains(bn) then
      self:check_suite(bn)
    end
  end
end

return {
  AUPUnits = AUPUnits
}
