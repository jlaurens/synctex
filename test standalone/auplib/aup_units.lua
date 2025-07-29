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
The individual tests are organized in a tree by `mode`, `year`, `suite` and `unit`.
`mode` is one of `engine` or `library`. Test that do not depend on the engine
are `library` tests, the others are `engine` ones.
The `year` is necessary because of the evolution of engines.
There is a `query` object to activate or deactivate subtrees.
On start, all the tests are deactivated.
--]==]

local AUP = package.loaded.AUP

local dbg = AUP.dbg

local PL = AUP.PL

local PL_class = PL.class

local PL_path = PL.path
local currentdir = PL_path.currentdir

local PL_List = PL.List
local PLOrderedMap = PL.OrderedMap

local state = AUP.state

--- @alias StringsByString { string: string[] }

--- @class os
--- @field type string
--- @field name string

--- This corresponds to each `test.lua` files of the `test standalone` folder.
---
--- We only consider 4 levels of subdirectories.
--- Year are modeled as strings because we just compare years
--- and do no arithmetics.
--- `AUPTest` stands for unit "test".
--- @class AUPTest
--- @field map fun(root: string): PLMap
--- @field path string
--- @field mode string
--- @field year string
--- @field suite string
--- @field unit string
--- @field visited boolean
--- @field excluded boolean
--- @field already_checked fun(self: AUPTest, session_id: string, tmp_dir: string): boolean
--- @field checked boolean whether checking of the receiver has started
local AUPTest = PL_class()

local countAUPTest = 1
--- Initialize an `AUPTest` instance
---
--- Straightforward... This should not be used directly, only
--- through the "static" methods `AUPTest.map`.
---@param path string
---@param mode string?
---@param year string?
---@param suite string?
---@param unit string?
function AUPTest:_init(path, mode, year, suite, unit)
  self.count = countAUPTest
  countAUPTest = countAUPTest+1
  self.path = path
  self.mode = mode or '.'
  self.year = year or '.'
  self.suite = suite or '.'
  self.unit = unit or '.'
  self.visited = false
  self.excluded = true
  self.checked = false
end

function AUPTest:__tostring()
  return 'AUPTest(excluded=%s): %s'%{self.excluded and 'Y' or 'N', self.path}
end

--- Get the map of unit tests by path at the given root path
---
--- This navigates through the FS tree to find `test.lua` files.
--- For each file found, the given directory is recorded in a 
--- created `AUPTest`.
--- This function is meant to be called ony once for a given root,
--- and it is meaningless to call it for subtrees.
---@param root string
---@return PLOrderedMap of AUPTest
function AUPTest.map(root)
  local ans = PLOrderedMap()
  AUP.pushd_or_raise(root, 'root')
  local p_mode = '.'
  if PL_path.exists('test.lua') then
    ans:set(p_mode, AUPTest(p_mode))
  end
  for mode in PL.seq.list {'engine', 'library'} do
    p_mode = 'test_'..mode
    AUP.pushd_or_raise(p_mode, 'mode')
    if PL_path.exists('test.lua') then
      ans:set(p_mode, AUPTest(p_mode, mode))
    end
    for year in PL_path.dir('.') do
      if string.match(year, '^%d%d%d%d$') then
        AUP.pushd_or_raise(year, 'year')
        local p1 = PL_path.join(p_mode, year)
        if PL_path.exists('test.lua') then
          ans:set(p1, AUPTest(p1, mode, year))
        end
        for suite in PL_path.dir('.') do
          if not string.match(suite, '^%.') and AUP.pushd(suite, 'suite') then
            local p2 = PL_path.join(p1, suite)
            if PL_path.exists('test.lua') then
              ans:set(p2, AUPTest(p2, mode, year, suite))
            end
            for unit in PL_path.dir('.') do
              if not string.match(unit, '^%.') and AUP.pushd(unit, 'unit') then
                if PL_path.exists('test.lua') then
                  local p3 = PL_path.join(p2, unit)
                  ans:set(p3, AUPTest(p3, mode, year, suite, unit))
                end
                AUP.popd_or_raise('unit') -- AUP.pushd(unit)
              end
            end
            AUP.popd_or_raise('suite') -- AUP.pushd(suite)
          end
        end
        AUP.popd_or_raise('year') -- AUP.pushd_or_raise(year)
      end
    end
    AUP.popd_or_raise('mode') -- AUP.pushd_or_raise(p_mode)
  end
  AUP.popd_or_raise('root') -- AUP.pushd_or_raise(root)
  ans:sort()
  return ans
end

--- @enum AUPUnitsQueryAction
AUPUnitsQueryAction = {
  Include   = 'include',
  IncludeExcept   = 'include except',
  Exclude   = 'exclude',
  ExcludeExcept   = 'exclude except'
}

--- @enum (key) AUPUnitsQueryMatch
AUPUnitsQueryMatch = {
  Any   = '\033.aup_units.Any.'..math.random(),
  Dot   = '\033.aup_units.Dot.'..math.random()
}

--- @class AUPUnitsQuery
--- @field modes PL_List of strings
--- @field years PL_List of strings
--- @field suites PL_List of strings
--- @field units PL_List of strings
--- @field matches fun(self: AUPUnitsQuery, test: AUPTest): boolean
--- @field apply fun(self: AUPUnitsQuery, tests: PLMap, action: AUPUnitsQueryAction, all: boolean?)
local AUPUnitsQuery = PL_class()

function AUPUnitsQuery:_init()
  self.modes = PL_List{}
  self.years = PL_List()
  self.suites = PL_List()
  self.units = PL_List()
end

function AUPUnitsQuery:__tostring()
  local ans = PL.pretty.write(self)
  return 'AUPUnitsQuery: '..ans
end

--- Whether the argument matches the receiver
---
--- The individual test units are logically organized in a tree
--- by `mode`, `year`, `suite` and `unit`.
--- 
---@param test AUPTest
---@return boolean
function AUPUnitsQuery:matches(test)
  local matcher = function(k, x)
    if k == AUPUnitsQueryMatch.Dot then
      return true, x == '.'
    elseif k == AUPUnitsQueryMatch.Any then
      return false, true
    else
      return false, x == k
    end
  end
  for mode in self.modes:iter() do
    local match_dot, match_else = matcher(mode, test.mode)
    if match_dot then return match_else end
    if match_else then
      for year in self.years:iter() do
        match_dot, match_else = matcher(year, test.year)
        if match_dot then return match_else end
        if match_else then
          for suite in self.suites:iter() do
            match_dot, match_else = matcher(suite, test.suite)
            if match_dot then return match_else end
            if match_else then
              for unit in self.units:iter() do
                match_dot, match_else = matcher(unit, test.unit)
                return match_dot or match_else
              end
            end
          end
        end
      end
    end
  end
  return false
end

--- Modify the argument in place to include the tests matching the receiver.
---
--- The individual test units are logically organized in a tree
--- by `mode`, `year`, `suite` and `unit`.
--- 
---@param tests PLMap
---@param action AUPUnitsQueryAction
---@param all boolean? true if all the tests must be matched and not only those that are not visited.
function AUPUnitsQuery:apply(tests, action, all)
  if not self._has_applied then
    self._has_applied = true
    local before_non_empty = true
    for k in PL.seq.list {'units', 'suites', 'years', 'modes'} do
      if #self[k] == 0 then
        if before_non_empty then
          self[k]:append(AUPUnitsQueryMatch.Dot)
        else
          self[k]:append(AUPUnitsQueryMatch.Any)
        end
      else
        before_non_empty = false
      end
    end
  end
  for test in tests:values():iter() do
    if all or not test.visited then
      if action == AUPUnitsQueryAction.Exclude then
        if self:matches(test) then
          test.excluded = true
        end
      elseif action == AUPUnitsQueryAction.ExcludeExcept then
        if not self:matches(test) then
          test.excluded = true
        end
      elseif action == AUPUnitsQueryAction.Include then
        if self:matches(test) then
          test.excluded = false
        end
      elseif action == AUPUnitsQueryAction.IncludeExcept then
        if not self:matches(test) then
          test.excluded = false
        end
      end
    end
  end
end

--- @class AUPArguments
--- @field unitsQueryInclude fun(self: AUPArguments): AUPUnitsQuery

local AUPArguments = AUP.Arguments

function AUPArguments:unitsQueryInclude()
  local ans = AUPUnitsQuery()
  local iterator = self:iterator()
  local entry = iterator:next()
  dbg:write(1, "**** Managing arguments")
  while(entry) do
    dbg:write(1, "entry: %s -> %s"%{entry.key, entry.value})
    local consume = false
    if not consume then
      for key in PL.seq.list {'engine', 'library'} do
        if entry.key == key then
          if entry.value == 'true' then
            if not ans.modes:contains(entry.key) then
              ans.modes:append(entry.key)
            end
          elseif entry.value == 'false' then
            while ans.modes:remove_value(entry.key) do end
          elseif entry:value_is_string() then
            error("Unsupported argument value %s"%{entry})
          elseif not ans.modes:contains(entry.key) then
            ans.modes:append(entry.key)
          end
          consume = true
          break
        end
      end
    end
    if not consume then
      for key in PL.seq.list {'mode', 'year', 'suite', 'unit'} do
        if entry.key == key then
          ans[entry.key..'s']:append(entry.value)
          iterator:consume()
          break
        end
      end
    end
    if not consume and string.match(entry.key, '^%d%d%d%d$') then
      if entry.value == 'true' then
        if not ans.years:contains(entry.key) then
          ans.years:append(entry.key)
        end
      elseif entry.value == 'false' then
        while ans.years:remove_value(entry.key) do end
      elseif entry:value_is_string() then
        error("Unsupported argument value %s"%{entry})
      elseif not ans.years:contains(entry.key) then
        ans.years:append(entry.key)
      end
    end
    if consume then
      iterator:consume()
    end
    entry = iterator:next()
  end
  return ans
end

--- @class AUPUnits
--- @field name string
--- @field session_id string
--- @field session_id_p string
--- @field dev boolean
--- @field dry boolean
--- @field build_dir string
--- @field tmp_dir string
--- @field _init fun(test_dir: string, arg: table)
--- @field setup_and_exit fun(self: AUPUnits)
--- @field setup_session_id fun(self: AUPUnits)
--- @field retrieve_session_id fun(self: AUPUnits)
--- @field teardown fun(self: AUPUnits)
--- @field tmp_dir_current fun(self: AUPUnits): string
--- @field skip fun(self: AUPUnits)
--- @field local_value fun(self: AUPUnits): string
--- @field get_test fun(path: string): AUPTest?
--- @field check fun(self: AUPUnits)
--- @field load fun(self: AUPUnits, name: string): boolean, string?
--- @field test_setup fun(self: AUPUnits)
--- @field test_teardown fun(self: AUPUnits)
--- @field test_setup_on_after fun(self: AUPUnits, f: any)
--- @field test_teardown_on_after fun(self: AUPUnits, f: (fun():any))
--- @field test_currentdir fun(self: AUPUnits, exclude: table?)
--- @field fail fun(self: AUPUnits, message: string)
--- @field print_failed fun(self: AUPUnits): integer
--- @field _cwd string
--- @field test_mode string?
--- @field test_year string?
--- @field test_suite string?
--- @field test_unit string?
--- @field test_current AUPTest?
--- @field _failures PL_List
--- @field _testMap PLOrderedMap

local AUPUnits = PL_class()

--- @class AUPState
--- @field units_skip_test boolean?

--- @class AUPK
--- @field units_skip_test string

AUP.K.units_skip_test = 'units_skip_test'


--- Skip tests at this level
---
--- Call in a `test_setup` to skip the tests of the folder
--- Useful for example to skip some test when the year is not the expected one.
function AUPUnits:skip()
  self:test_foreach(function (test)
    test.excluded = true
  end, self.test_mode, self.test_year, self.test_suite, self.test_unit)
end

--- Initialize an `AUPUnits` instance
function AUPUnits:_init(test_dir, arg)
  PL.utils.assert_string(1, test_dir)
  PL.utils.assert_string(2, arg.build_dir)
  PL.utils.assert_string(2, arg.name)
  --self:super()   -- call the ancestor initializer if needed
  self.build_dir = arg.build_dir
  self.dev = arg.dev
  self.dry = arg.dry
  dbg:write(1,'units.dev: '..(self.dev and '⟨true⟩' or '⟨false⟩'))
  self.name = arg.name
  self.tmp_dir = PL_path.join(self.build_dir, 'tmp')
  PL.dir.makepath(self.tmp_dir)
  self.session_id_p = PL_path.join(self.tmp_dir, 'session_id')
  self:retrieve_session_id()
  -- we scan the file system for all the unit tests
  -- we start at the `.../test standalone` level
  -- we scan both `test_engine` and `test_library` folders
  self._testMap = AUPTest.map(test_dir)
  dbg:printf(10, "%s\n", self)
  self._cwd = currentdir()
  self._failures = PL_List.new()
end

--- String representation
--- @return string
function AUPUnits:__tostring()
  local ans = PL.pretty.write(self)
  return 'AUPUnits: '..ans
end

--- The key used for the local configuration
---
--- The local configuration is necessary to take into account
--- that development setup is not unique and depends on the developer
--- at least.
--- One shot function.
--- Returns the `⟨value⟩` in CLI argument `--local=⟨value⟩`.
--- The various `test_setup_⟨value⟩.lua` and `test_teardown_⟨value⟩.lua`
--- are read before and after tests when provided.
--- The corresponding argument entry is consumed.
---@return string?
function AUPUnits:local_value()
  local ans = AUP.arguments.local_value
  local function f()
    local cached = #ans > 0 and ans or nil
    return cached, function () return cached end
  end
  ans, self.local_value = f()
  return ans
end

--- Overall setup and exit
---
--- Expected to be called once per test session.
--- Prints a unique id for that test.
--- This id is used to build temporary directories that are used
--- as locks.
function AUPUnits:setup_and_exit()
  -- acquire a unique conversation id
  self:setup_session_id()
  os.exit(0)
end

--- Set up a new session id
function AUPUnits:setup_session_id()
  -- acquire a unique conversation id
  local session_id = 1
  AUP.pushd(self.tmp_dir, 'setup_and_exit')
  if PL_path.exists(self.session_id_p) then
    local s = PL.file.read(self.session_id_p)
    local id = PL.stringx.split(s, 'session_id:')[2]
    if id then
      local n = tonumber(id)
      if n ~= nil then
        session_id = n+1
      end
    end
  end
  print('setup_session_id: '..session_id)
  PL.file.write(self.session_id_p, 'session_id:'..session_id..'\n')
  AUP.popd('setup_and_exit')
end

--- Set up a new session id
function AUPUnits:retrieve_session_id()
  -- acquire a unique conversation id
  local session_id = 1
  AUP.pushd(self.tmp_dir, 'retrieve_session_id')
  if PL_path.exists(self.session_id_p) then
    local s = PL.file.read(self.session_id_p)
    local id = PL.stringx.split(s, 'session_id:')[2]
    if id then
      local n = tonumber(id)
      if n ~= nil then
        session_id = n
      end
    end
  end
  self.session_id = string.sub('0000'..session_id, -4)
  AUP.popd('retrieve_session_id')
end

--- Overall teardown
---
--- Expected to be called once per test session.
--- Called by the `teardown` test.
--- Current implementation does nothing yet.
function AUPUnits:teardown()
end

--- Get an `AUPTest` instance given its path.
---
---All `AUPTest` are created all at once in the test setup.
---@param path string?
---@return AUPTest?
function AUPUnits:get_test(path)
  if #path == 0 then
    return self._testMap:get('.')
  end
  return self._testMap:get(PL_path.normpath(path))
end

--- Load a file in the current directory.
--- Class method.
--- @param name string
--- @return boolean
--- @return (string|nil)?
function AUPUnits:load(name)
  local f, msg = loadfile(name..'.lua')
  if f ~= nil then
    local p = PL_path.abspath(name..'.lua')
    local cwd_before = PL_path.currentdir()
    if dbg:level_get()>1 then
      print("Loading: `"..(AUP.short_path(p)).."`")
    else
      dbg:write(1,"Loading: `"..name..'.lua`')
    end
    f()
    local cwd_after = PL_path.currentdir()
    if cwd_after ~= cwd_before then
      print("Warning about cwd:\n  before: %s\n  after:  %s"%{cwd_before, cwd_after})
    end
    if dbg:level_get()>1 then
      print("Loaded:  `"..(AUP.short_path(p)).."`")
    else
      dbg:write(1,"Loaded:  `"..name..'.lua`')
    end
    return true
  else
    dbg:write(1, "No `"..name..'.lua` at '..AUP.short_path())
    return false, msg
  end
end

--- @class AUPState
--- @field test_setup_after PL_List
--- @field test_teardown_after PL_List
--- @field test_setup_on_after_saved fun(self: AUPUnits, f: fun())
--- @field test_teardown_on_after_saved fun(self: AUPUnits, f: fun())

--- @class AUPK
--- @field test_setup_after string
--- @field test_teardown_after string
--- @field test_setup_on_after_saved string
--- @field test_teardown_on_after_saved string

AUP.K.test_setup_after = 'test_setup_after'
AUP.K.test_teardown_after = 'test_teardown_after'
AUP.K.test_setup_on_after_saved = 'test_setup_on_after_saved'
AUP.K.test_teardown_on_after_saved = 'test_teardown_on_after_saved'

--- @class AUPUnits
--- @field print fun(self: AUPUnits, ...)

---Prints nothing in `dry` mode.
---@param ... unknown
function AUPUnits:print(...)
  if not self.dry then
    print(...)
  end
end
--- Load the `test_setup.lua` of the current directory, if any.
function AUPUnits:test_setup()
  self:print('Test setup for '..AUP.short_path())
  state:setup()
  -- first setup the temporary directory that will be returned by
  -- `tmp_dir_current`, this is done only when there a test
  -- for the actual current working directory.
  local name = 'test_setup'
  local l = PL_List({name})
  ---@diagnostic disable-next-line: undefined-field
  name = name..'_'..os.type
  l:append(name)
  ---@diagnostic disable-next-line: undefined-field
  name = name..'_'..os.name
  l:append(name)
  local v = self:local_value()
  if v then
    l:append('test_setup_local_'..v)
  end
  state:set(AUP.K.test_setup_after, PL_List())
  state:set(AUP.K.test_setup_on_after_saved, self.test_setup_on_after)
  function self:test_setup_on_after(f)
    state:get(AUP.K.test_setup_after):append(f)
  end
  state:set(AUP.K.test_teardown_after, PL_List())
  state:set(AUP.K.test_teardown_on_after_saved, self.test_teardown_on_after)
  function self:test_teardown_on_after(f)
    state:get(AUP.K.test_teardown_after):append(f)
  end
  local status, code = pcall(function()
    for n in l:iter() do
      self:load(n)
    end
    for f in state:get(AUP.K.test_setup_after):iter() do
      f()
    end
  end)
  if not status then
    error(code)
  end
  self.test_setup_on_after = state:get(AUP.K.test_setup_on_after_saved)
end

--- Postpone the execution of a function.
function AUPUnits:test_setup_on_after(f)
  error('Use this function only from a `test_setup...` file', 2)
end

--- Postpone the execution of a function.
function AUPUnits:test_teardown_on_after(f)
  error('Use this function only from a `test_setup...` or a `test_teardown...` file')
end

--- Load the `test_teardown.lua` of the current directory, if any.
function AUPUnits:test_teardown()
  self:print('Test teardown for '..AUP.short_path())
  local name = 'test_teardown'
  local l = PL_List({name})
---@diagnostic disable-next-line: undefined-field
  name = name..'_'..os.type
  l:put(name)
  ---@diagnostic disable-next-line: undefined-field
  name = name..'_'..os.name
  l:put(name)
  local v = self:local_value()
  if v then
    l:put('test_teardown_local_'..v)
  end
  local status, error_msg = pcall(function()
    for n in l:iter() do
      self:load(n)
    end
    for f in state:listGet(AUP.K.test_teardown_after):iter() do
      f()
    end
  end)
  self.test_teardown_on_after = state:get(AUP.K.test_teardown_on_after_saved)
  state:teardown()
  if not status then
    error(error_msg)
  end
end

--- Run all the tests
--- @param self AUPUnits
function AUPUnits:check()
  dbg:write(999, 'AUPUnits:check...')
  self:test_setup()
  local skip = state:get(AUP.K.units_skip_test)
  if skip ~= nil and not skip then
    self:test_teardown()
    dbg:write(9999, 'AUPUnits:check... SKIPPED')
    return
  end
  --- @type AUPUnitsQuery
  local query = AUP.arguments:unitsQueryInclude()
  query:apply(self._testMap, AUPUnitsQueryAction.Include)
  query = AUPUnitsQuery()
  query.modes:append(AUPUnitsQueryMatch.Any)
  query.years:append('2222')
  query.suites:append(AUPUnitsQueryMatch.Any)
  query.units:append(AUPUnitsQueryMatch.Any)
  if self.dev then
    local ans = self:test_foreach(function(t) if not t.excluded then return true, true end end)
    if not ans then
      query:apply(self._testMap, AUPUnitsQueryAction.Include)
    end
  else
    query:apply(self._testMap, AUPUnitsQueryAction.Exclude)
  end
  if dbg:level_get() > 999 then
    PL.pretty(self._testMap)
  end
  self.test_current = nil
  self.test_mode = nil
  self.test_year = nil
  self.test_suite = nil
  self.test_unit = nil
  local function f(t)
    for k in PL.seq.list(t) do
      k = 'test_'..k
      if self[k] ~= nil then
        self:test_teardown()
        AUP.popd_or_raise(k)
        self[k] = nil
      end
    end
  end
  for test in self._testMap:values():iter() do
    dbg:write(99, 'test candidate: '..tostring(test))
    dbg:write(99, '  self.test_mode='..(self.test_mode or 'nil'))
    dbg:write(99, '  self.test_year='..(self.test_year or 'nil'))
    dbg:write(99, '  self.test_suite='..(self.test_suite or 'nil'))
    dbg:write(99, '  self.test_unit='..(self.test_unit or 'nil'))
    test.visited = true
    if not test.excluded and not test.checked then
      local tmp = 'tmp_'..self.session_id
      local p = PL_path.normpath(PL_path.join(self.tmp_dir, test.path, tmp))
      if PL_path.exists(p) then
        test.visited = true
        dbg:write(1, 'Already visited: '..test.path)
      else
        dbg:write(1, 'Visiting: '..test.path)
        self.test_current = test
        self:print('▶︎ tmp: '..tmp)
        PL.dir.makepath(p)
        state:set(AUP.K.tmp_dir, p)
        if self.test_mode ~= test.mode then
          f({'unit', 'suite', 'year', 'mode'})
          AUP.pushd_or_raise('test_'..test.mode, 'test_mode')
          self.test_mode = test.mode
          self:print('▶︎ Test mode: '..self.test_mode)
          self:test_setup()
        end
        if test.year ~= '.' then
          if self.test_year ~= test.year then
            f({'unit', 'suite', 'year'})
            AUP.pushd_or_raise(test.year, 'test_year')
            self.test_year = test.year
            self:print('▶︎   Test year: '..self.test_year)
            self:test_setup()
          end
          if test.suite ~= '.' then
            if self.test_suite ~= test.suite then
              f({'unit', 'suite'})
              AUP.pushd_or_raise(test.suite, 'test_suite')
              self.test_suite = test.suite
              self:print('▶︎     Test suite: '..self.test_suite)
              self:test_setup()
            end
            if test.unit ~= '.' then
              if self.test_unit ~= test.unit then
                f({'unit'})
                AUP.pushd_or_raise(test.unit, 'test_unit')
                self.test_unit = test.unit
                self:print('▶︎       Test unit: '..self.test_unit)
                self:test_setup()
              end
            end
          end
        end
        self:print('Test for '..test.path)
        if self.dry then
          print('Dry visit: '..test.path)
        else
          self:load('test')
          dbg:write(1, 'Visited: '..test.path)
        end
        self.test_current = nil
      end
    end
  end
  f({'unit', 'suite', 'year', 'mode'})
  self:test_teardown()
  dbg:write(999, 'AUPUnits:check... DONE')
end

--- @class AUPUnits
--- @field test_foreach fun(self: AUPUnits, action: fun(x: AUPTest): boolean?, any?, mode: string?, year: string?, suite: string?, unit: string?)

--- Run an action on each test
--- @param action fun(x: AUPTest): boolean?, any?
--- @param mode string?
--- @param year string?
--- @param suite string?
--- @param unit string?
function AUPUnits:test_foreach(action, mode, year, suite, unit)
  for test in self._testMap:values():iter() do
    if not mode or mode == test.mode then
      if not year or year == test.mode then
        if not suite or suite == test.mode then
          if not unit or unit == test.mode then
            local status, ans = action(test)
            if status then
              return ans
            end
          end
        end
      end
    end
  end
end

--- Run all the tests in the given directory
---
--- `suite` is an optional argument.

--- Run tests in the current working directory.
---
--- Activate the tests in the current directory.
--- It makes sense when the current directory is not the deepest one.
--- @param exclude table?
function AUPUnits:test_currentdir(exclude)
  dbg:write(999, '▶︎▶︎▶︎ test_currentdir "'..AUP.short_path(PL_path.abspath('.'))..'".')
  dbg:write(999, 'self.test_mode: '..(self.test_mode or 'nil'))
  dbg:write(999, 'self.test_year: '..(self.test_year or 'nil'))
  dbg:write(999, 'self.test_suite: '..(self.test_suite or 'nil'))
  dbg:write(999, 'self.test_unit: '..(self.test_unit or 'nil'))
  local query = AUPUnitsQuery()
  query.modes = PL_List{AUPUnitsQueryMatch.Any}
  query.years = PL_List{AUPUnitsQueryMatch.Any}
  query.suites = PL_List{AUPUnitsQueryMatch.Any}
  query.units  = PL_List{AUPUnitsQueryMatch.Any}
  if self.test_mode == nil then
    query:apply(self._testMap, AUPUnitsQueryAction.Include)
    query.modes = PL_List(exclude)
    query:apply(self._testMap, AUPUnitsQueryAction.Exclude)
    return
  else
    query.modes = PL_List{self.test_mode}
    if self.test_year == nil then
      query:apply(self._testMap, AUPUnitsQueryAction.Include)
      query.years = PL_List(exclude)
      query:apply(self._testMap, AUPUnitsQueryAction.Exclude)
      return
    else
      query.years = PL_List{self.test_year}
      if self.test_suite == nil then
        query:apply(self._testMap, AUPUnitsQueryAction.Include)
        query.suites = PL_List(exclude)
        query:apply(self._testMap, AUPUnitsQueryAction.Exclude)
        return
      else
        query.suites = PL_List{self.test_suite}
        if self.test_unit == nil then
          query:apply(self._testMap, AUPUnitsQueryAction.Include)
          if #exclude>0 then
            AUPUnits._debug = 1
            query.units = PL_List(exclude)
            query:apply(self._testMap, AUPUnitsQueryAction.Exclude)
          end
          return
        else
          return
        end
      end
    end
  end
end

--- @class AUPK
--- @field tmp_dir string

AUP.K.tmp_dir = 'tmp_dir'

--- The current directory for temporary material.
---
--- Create the directory as side effect.
--- @return string
function AUPUnits:tmp_dir_current()
  local ans = state:get(AUP.K.tmp_dir)
  PL.dir.makepath(ans)
  return ans
end

--- Whether the test is excluded
---
--- Either because its `excluded` field is `true` or
--- because it has already been checked during this session.
---@param test AUPTest
---@return boolean
function AUPUnits:checking(test)
  return PL_path.exists(PL_path.join(self.tmp_dir, test.path, 'tmp_'..self.session_id))
end

--- Declares a failure.
--- @param message string
function AUPUnits:fail(message)
  self._failures:append({
    test = self.test_current,
    message = message,
  })
end

--- Declares a failure.
--- @return integer
function AUPUnits:print_failed()
  if self._failures:len() > 0 then
    print("FAIL:")
    for t in self._failures:iter() do
      local l = PL_List()
      if t.test then
        l:append('test: '..t.test.path)
      end
      if t.message then
        l:append(t.message)
      end
      print("  "..l:concat(" — "))
    end
  end
  return self._failures:len()
end

return {
  Units = AUPUnits,
  Test = AUPTest
}
