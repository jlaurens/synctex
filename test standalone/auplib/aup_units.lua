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

--- @class AUP
local AUP = package.loaded.AUP

local K = AUP.K

local dbg = AUP.dbg

local List       = require"pl.List"
local OrderedMap = require"pl.OrderedMap"
local pl_pretty  = require"pl.pretty"
local pl_utils   = require"pl.utils"
local pl_class   = require"pl.class"
local pl_dir     = require"pl.dir"
local pl_seq     = require"pl.seq"
local pl_path    = require"pl.path"
local pl_stringx = require"pl.stringx"
local pl_file    = require"pl.file"

local currentdir = lfs.currentdir
local assert_string = pl_utils.assert_string

--- @alias StringsByString { string: string[] }

--- This corresponds to each `test.lua` files of the `test standalone` folder.
---
--- We only consider 4 levels of subdirectories.
--- Year are modeled as strings because we just compare years
--- and do no arithmetics.
--- `AUP.Test` stands for unit "test".
--- @class AUP.Test: AUP.Class
--- @field path string
--- @field mode string
--- @field year string
--- @field suite string
--- @field unit string
--- @field visited boolean
--- @field excluded boolean
--- @field checked boolean whether checking of the receiver has started
local Test = pl_class()

AUP.Test = Test

local countAUPTest = 1
--- Initialize an `AUP.Test` instance
---
--- Straightforward... This should not be used directly, only
--- through the "static" methods `AUP.Test.map`.
--- @param path string
--- @param mode string?
--- @param year string?
--- @param suite string?
--- @param unit string?
function Test:_init(path, mode, year, suite, unit)
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

function Test:__tostring()
  return 'AUP.Test(excluded=%s): %s'%{self.excluded and 'Y' or 'N', self.path}
end

--- Get the map of unit tests by path at the given root path
---
--- This navigates through the FS tree to find `test.lua` files.
--- For each file found, the given directory is recorded in a 
--- created `AUP.Test`.
--- This function is meant to be called ony once for a given root,
--- and it is meaningless to call it for subtrees.
--- @param root string
--- @return pl.OrderedMap of AUP.Test
function Test.map(root)
  local ans = OrderedMap()
  AUP.pushd_or_raise(root, 'root')
  local p_mode = '.'
  if pl_path.exists('test.lua') then
    ans:set(p_mode, Test(p_mode))
  end
  for mode in pl_seq.list {'engine', 'library'} do
    p_mode = 'test_'..mode
    AUP.pushd_or_raise(p_mode, 'mode')
    if pl_path.exists('test.lua') then
      ans:set(p_mode, Test(p_mode, mode))
    end
    for year in pl_path.dir('.') do
      if string.match(year, '^%d%d%d%d$') then
        AUP.pushd_or_raise(year, 'year')
        local p1 = pl_path.join(p_mode, year)
        if pl_path.exists('test.lua') then
          ans:set(p1, Test(p1, mode, year))
        end
        for suite in pl_path.dir('.') do
          if not string.match(suite, '^%.') and AUP.pushd(suite, 'suite') then
            local p2 = pl_path.join(p1, suite)
            if pl_path.exists('test.lua') then
              ans:set(p2, Test(p2, mode, year, suite))
            end
            for unit in pl_path.dir('.') do
              if not string.match(unit, '^%.') and AUP.pushd(unit, 'unit') then
                if pl_path.exists('test.lua') then
                  local p3 = pl_path.join(p2, unit)
                  ans:set(p3, Test(p3, mode, year, suite, unit))
                end
                AUP.popd_or_raise('unit')
              end
            end
            AUP.popd_or_raise('suite')
          end
        end
        AUP.popd_or_raise('year')
      end
    end
    AUP.popd_or_raise('mode')
  end
  AUP.popd_or_raise('root')
  ans:sort()
  return ans
end

--- @class AUP.Units: AUP.State.Compliant
--- @field super fun(self: AUP.State.Compliant)
local Units = pl_class(AUP.State.Compliant)

AUP.Units = Units

--- @enum AUP.Units.QueryAction
Units.QueryAction = {
  Include         = 'include',
  IncludeExcept   = 'include except',
  Exclude         = 'exclude',
  ExcludeExcept   = 'exclude except'
}

--- @enum (key) AUP.Units.QueryMatch
Units.QueryMatch = {
  Any   = '\033.aup_units.Any.'..math.random(),
  Dot   = '\033.aup_units.Dot.'..math.random()
}

-- A query object allows to filter the tests.
--- @class AUP.Units.Query: AUP.Class
--- @field modes pl.List of strings
--- @field years pl.List of strings
--- @field suites pl.List of strings
--- @field units pl.List of strings
local Query = pl_class()

Units.Query = Query

function Query:_init()
  self.modes = List{}
  self.years = List()
  self.suites = List()
  self.units = List()
end

function Query:__tostring()
  local ans = pl_pretty(self)
  return 'AUP.Units.Query: '..ans
end

--- Whether the argument matches the receiver
---
--- The individual test units are logically organized in a tree
--- by `mode`, `year`, `suite` and `unit`.
--- 
--- @param test AUP.Test
--- @return boolean
function Query:matches(test)
  local matcher = function(k, x)
    if k == AUP.Units.QueryMatch.Dot then
      return true, x == '.'
    elseif k == AUP.Units.QueryMatch.Any then
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
--- @param tests pl.Map
--- @param action AUP.Units.QueryAction
--- @param all boolean? true if all the tests must be matched and not only those that are not visited.
function Query:apply(tests, action, all)
  if not self._has_applied then
    self._has_applied = true
    local before_non_empty = true
    for k in pl_seq.list {'units', 'suites', 'years', 'modes'} do
      if #self[k] == 0 then
        if before_non_empty then
          self[k]:append(AUP.Units.QueryMatch.Dot)
        else
          self[k]:append(AUP.Units.QueryMatch.Any)
        end
      else
        before_non_empty = false
      end
    end
  end
--- @diagnostic disable-next-line: undefined-field
  for test in tests:values():iter() do -- https://github.com/goldenstein64/pl-definitions/pull/1
    if all or not test.visited then
      if action == AUP.Units.QueryAction.Exclude then
        if self:matches(test) then
          test.excluded = true
        end
      elseif action == AUP.Units.QueryAction.ExcludeExcept then
        if not self:matches(test) then
          test.excluded = true
        end
      elseif action == AUP.Units.QueryAction.Include then
        if self:matches(test) then
          test.excluded = false
        end
      elseif action == AUP.Units.QueryAction.IncludeExcept then
        if not self:matches(test) then
          test.excluded = false
        end
      end
    end
  end
end

--- @class AUP.Arguments
local Arguments = AUP.Arguments

--- Include query
--- @return AUP.Units.Query
function Arguments:unitsQueryInclude()
  local ans = AUP.Units.Query()
  local iterator = self:iterator()
  local entry = iterator:next()
  dbg:write(1, "**** Managing arguments")
  while(entry) do
    dbg:write(1, "entry: %s -> %s"%{entry.key, entry.value})
    local consume = false
    if not consume then
      for key in pl_seq.list {'engine', 'library'} do
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
      for key in pl_seq.list {'mode', 'year', 'suite', 'unit'} do
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

--- @class AUP.Units
--- @field name string
--- @field session_id string
--- @field session_id_p string
--- @field dev boolean
--- @field dry boolean
--- @field build_dir string
--- @field tmp_dir string
--- @field _cwd string
--- @field test_mode string?
--- @field test_year string?
--- @field test_suite string?
--- @field test_unit string?
--- @field test_current AUP.Test?
--- @field _failures pl.List
--- @field _testMap pl.OrderedMap

--- @class AUP.State
--- @field units_skip_test boolean?

--- @class AUP.K
--- @field units_skip_test 'units_skip_test'

K.units_skip_test = 'units_skip_test'


--- Skip tests at this level
---
--- Call in a `test_setup` to skip the tests of the folder
--- Useful for example to skip some test when the year is not the expected one.
function Units:skip()
  self:test_foreach{
    action = function (test)
      test.excluded = true
    end,
    mode = self.test_mode,
    year = self.test_year,
    suite = self.test_suite,
    unit = self.test_unit
  }
end

--- Initialize an `AUP.Units` instance
function Units:_init(test_dir, arg)
  assert_string(1, test_dir)
  assert_string(2, arg.build_dir)
  assert_string(2, arg.name)
  self:super()
  --self:super()   -- call the ancestor initializer if needed
  self.build_dir = arg.build_dir
  self.dev = arg.dev
  self.dry = arg.dry
  dbg:write(1,'units.dev: '..(self.dev and '⟨true⟩' or '⟨false⟩'))
  self.name = arg.name
  self.tmp_dir = pl_path.join(self.build_dir, 'tmp')
  pl_dir.makepath(self.tmp_dir)
  self.session_id_p = pl_path.join(self.tmp_dir, 'session_id')
  self:retrieve_session_id()
  -- we scan the file system for all the unit tests
  -- we start at the `.../test standalone` level
  -- we scan both `test_engine` and `test_library` folders
  self._testMap = Test.map(test_dir)
  dbg:printf(10, "%s\n", self)
  self._cwd = assert(currentdir())
  self._failures = List.new()
  self._skipures = List.new()
end

--- String representation
--- @return string
function Units:__tostring()
  local ans = pl_pretty(self)
  return 'AUP.Units: '..ans
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
--- @return string?
function Units:local_value()
  --- @type string?
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
function Units:setup_and_exit()
  -- acquire a unique conversation id
  self:setup_session_id()
  os.exit(0)
end

--- Set up a new session id
function Units:setup_session_id()
  -- acquire a unique conversation id
  local session_id = 1
  AUP.pushd(self.tmp_dir, 'setup_and_exit')
  if pl_path.exists(self.session_id_p) then
    local s = pl_file.read(self.session_id_p)
    local id = pl_stringx.split(s, 'session_id:')[2]
    if id then
      local n = tonumber(id)
      if n ~= nil then
        session_id = n+1
      end
    end
  end
  print('setup_session_id: '..session_id)
  pl_file.write(self.session_id_p, 'session_id:'..session_id..'\n')
  AUP.popd('setup_and_exit')
end

--- Set up a new session id
function Units:retrieve_session_id()
  -- acquire a unique conversation id
  local session_id = 1
  AUP.pushd(self.tmp_dir, 'retrieve_session_id')
  if pl_path.exists(self.session_id_p) then
    local s = pl_file.read(self.session_id_p)
    local id = pl_stringx.split(s, 'session_id:')[2]
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
function Units:teardown()
end

--- Get an `AUP.Test` instance given its path.
---
---All `AUP.Test` are created all at once in the test setup.
--- @param path string?
--- @return AUP.Test?
function Units:get_test(path)
  if path and #path > 0 then
    return self._testMap:get(pl_path.normpath(path))
  end
  return self._testMap:get('.')
end

--- Load a file in the current directory.
--- Class method.
--- @param name string
--- @return boolean
--- @return string?
function Units:load(name)
  assert_string(2, name)
  name = name..".lua"
  local p = AUP.short_path(pl_path.abspath(name))
  local f, msg = loadfile(name)
  if f ~= nil then
    local cwd_before = lfs.currentdir()
    if dbg:level_get()>1 then
      print("Loading: `"..p.."`")
    else
      dbg:write(1,"Loading: `"..name..'`')
    end
    f()
    local cwd_after = lfs.currentdir()
    if cwd_after ~= cwd_before then
      print("Warning about cwd:\n  before: %s\n  after:  %s"%{cwd_before, cwd_after})
    end
    if dbg:level_get()>1 then
      print("Loaded:  `"..p.."`")
    else
      dbg:write(1,"Loaded:  `"..name..'`')
    end
    return true
  elseif pl_path.exists(p) then
    error("Corrupted `"..p.."`")
  else
    dbg:write(1, "No `"..name..'`')
    return false, msg
  end
end

--- @class AUP.State
--- @field test_setup_after pl.List
--- @field test_teardown_after pl.List
--- @field test_setup_on_after_saved fun(self: AUP.Units, f: fun())
--- @field test_teardown_on_after_saved fun(self: AUP.Units, f: fun())

--- @class AUP.K
--- @field test_setup_after 'test_setup_after'
--- @field test_teardown_after 'test_teardown_after'
--- @field test_setup_on_after_saved 'test_setup_on_after_saved'
--- @field test_teardown_on_after_saved 'test_teardown_on_after_saved'

K.test_setup_after = 'test_setup_after'
K.test_teardown_after = 'test_teardown_after'
K.test_setup_on_after_saved = 'test_setup_on_after_saved'
K.test_teardown_on_after_saved = 'test_teardown_on_after_saved'

---Prints nothing in `dry` mode.
--- @param ... unknown
function Units:print(...)
  if not self.dry then
    print(...)
  end
end

--- @class AUP.Units
--- @field tmp_dir_current fun(self: AUP.Units): string
--- @field skip fun(self: AUP.Units)
--- @field local_value fun(self: AUP.Units): string
--- @field check fun(self: AUP.Units)
--- @field load fun(self: AUP.Units, name: string): boolean, string?
--- @field test_teardown fun(self: AUP.Units)
--- @field test_setup_on_after fun(self: AUP.Units, f: any)
--- @field test_teardown_on_after fun(self: AUP.Units, f: (fun():any))

--- @class oslib
--- @field name string

--- Load the `test_setup.lua` of the current directory, if any.
function Units:test_setup()
  self:print('Test setup for '..AUP.short_path())
  self:state_setup()
  -- first setup the temporary directory that will be returned by
  -- `tmp_dir_current`, this is done only when there a test
  -- for the actual current working directory.
  local name = 'test_setup'
  local l = List({name})
  name = name..'_'..os.type
  l:append(name)
  name = name..'_'..os.name
  l:append(name)
  local v = self:local_value()
  if v then
    if #v > 0 then
      l:append('test_setup_local_'..v)
    else
      l:append('test_setup_local')
    end
  end
  print("DEBUG K.test_setup_after", K.test_setup_after)
  self:state_set(K.test_setup_after, List())
  self:state_set(K.test_setup_on_after_saved, self.test_setup_on_after)
  function self:test_setup_on_after(f)
    self:state_get(K.test_setup_after):append(f)
  end
  self:state_set(K.test_teardown_after, List())
  self:state_set(K.test_teardown_on_after_saved, self.test_teardown_on_after)
  function self:test_teardown_on_after(f)
    self:state_get(K.test_teardown_after):append(f)
  end
  local status, code = pcall(function()
    for n in l:iter() do
      self:load(n)
    end
    for f in self:state_get(K.test_setup_after):iter() do
      f()
    end
  end)
  self.test_setup_on_after = self:state_get(K.test_setup_on_after_saved)
  if not status then
    error(code)
  end
end

--- Postpone the execution of a function.
function Units:test_setup_on_after(f)
  error('Use this function only from a `test_setup...` file', 2)
end

--- Postpone the execution of a function.
function Units:test_teardown_on_after(f)
  error('Use this function only from a `test_setup...` or a `test_teardown...` file')
end

--- Load the `test_teardown.lua` of the current directory, if any.
function Units:test_teardown()
  self:print('Test teardown for '..AUP.short_path())
  local name = 'test_teardown'
  local l = List({name})
  --- @diagnostic disable-next-line: undefined-field
  name = name..'_'..os.type
  l:put(name)
  --- @diagnostic disable-next-line: undefined-field
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
    for f in self:state_get_List(K.test_teardown_after):iter() do
      f()
    end
  end)
  self.test_teardown_on_after = self:state_get(K.test_teardown_on_after_saved)
  self:state_teardown()
  if not status then
    error(error_msg)
  end
end

--- Run all the selected tests
function Units:check()
  dbg:write(999, 'AUP.Units:check...')
  self:test_setup()
  local skip = self:state_get_boolean(K.units_skip_test)
  if skip ~= nil and not skip then
    self:test_teardown()
    dbg:write(9999, 'AUP.Units:check... SKIPPED')
    return
  end
  local query = AUP.arguments:unitsQueryInclude()
  query:apply(self._testMap, AUP.Units.QueryAction.Include)
  print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
  self:test_foreach{action = function(t) if not t.excluded then print('Test: %s'%{t.path}) end end}
  print("????????????????????????????????????????")
  -- the 2222 query
  query = Query()
  query.modes:append(AUP.Units.QueryMatch.Any)
  query.years:append('2222')
  query.suites:append(AUP.Units.QueryMatch.Any)
  query.units:append(AUP.Units.QueryMatch.Any)
  if self.dev then
    query:apply(self._testMap, AUP.Units.QueryAction.ExcludeExcept)
    self:test_foreach{action = function(t) if not t.excluded then print('Test: %s'%{t.path}) end end}
    print("++++++++++++++++++++++++++++++++++++++++")
    local ans = self:test_foreach{action = function(t) if not t.excluded then return true, true end end}
    if not ans then
      query:apply(self._testMap, AUP.Units.QueryAction.Include)
      self:test_foreach{action = function(t) if not t.excluded then print('Test: %s'%{t.path}) end end}
      print("----------------------------------------")
    end
  else
    query:apply(self._testMap, AUP.Units.QueryAction.Exclude)
  end
  if dbg:level_get() > 999 then
    pl_pretty(self._testMap)
  end
  self.test_current = nil
  self.test_mode = nil
  self.test_year = nil
  self.test_suite = nil
  self.test_unit = nil
  local function f(t)
    for k in pl_seq.list(t) do
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
      local p = pl_path.normpath(pl_path.join(self.tmp_dir, test.path, tmp))
      if pl_path.exists(p) then
        test.visited = true
        dbg:write(1, 'Already visited: '..test.path)
      else
        dbg:write(1, 'Visiting: '..test.path)
        self.test_current = test
        self:print('▶︎ tmp: '..tmp)
        pl_dir.makepath(p)
        self:state_set(K.tmp_dir, p)
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
  dbg:write(999, 'AUP.Units:check... DONE')
end

--- @class AUPUnits_amysu
--- @field action fun(x: AUP.Test): boolean?, any?
--- @field mode string?
--- @field year string?
--- @field suite string?
--- @field unit string?

--- Run an action on each test
--- @param arg AUPUnits_amysu
function Units:test_foreach(arg)
--- @diagnostic disable-next-line: undefined-field
  for test in self._testMap:values():iter() do
    if arg.mode == nil or arg.mode == test.mode then
      if arg.year == nil or arg.year == test.mode then
        if arg.suite == nil or arg.suite == test.mode then
          if arg.unit == nil or arg.unit == test.mode then
            local status, ans = arg.action(test)
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
function Units:test_currentdir(exclude)
  dbg:write(999, '▶︎▶︎▶︎ test_currentdir "'..AUP.short_path(pl_path.abspath('.'))..'".')
  dbg:write(999, '  self.test_mode: ' ..(self.test_mode or 'nil'))
  dbg:write(999, '  self.test_year: ' ..(self.test_year or 'nil'))
  dbg:write(999, '  self.test_suite: '..(self.test_suite or 'nil'))
  dbg:write(999, '  self.test_unit: ' ..(self.test_unit or 'nil'))
  local query = Query()
  query.modes = List{AUP.Units.QueryMatch.Any}
  query.years = List{AUP.Units.QueryMatch.Any}
  query.suites = List{AUP.Units.QueryMatch.Any}
  query.units  = List{AUP.Units.QueryMatch.Any}
  if self.test_mode == nil then
    query:apply(self._testMap, AUP.Units.QueryAction.Include)
    query.modes = List(exclude)
    query:apply(self._testMap, AUP.Units.QueryAction.Exclude)
    return
  else
    query.modes = List{self.test_mode}
    if self.test_year == nil then
      query:apply(self._testMap, AUP.Units.QueryAction.Include)
      query.years = List(exclude)
      query:apply(self._testMap, AUP.Units.QueryAction.Exclude)
      return
    else
      query.years = List{self.test_year}
      if self.test_suite == nil then
        query:apply(self._testMap, AUP.Units.QueryAction.Include)
        query.suites = List(exclude)
        query:apply(self._testMap, AUP.Units.QueryAction.Exclude)
        return
      else
        query.suites = List{self.test_suite}
        if self.test_unit == nil then
          query:apply(self._testMap, AUP.Units.QueryAction.Include)
          if #exclude>0 then
            Units._debug = 1
            query.units = List(exclude)
            query:apply(self._testMap, AUP.Units.QueryAction.Exclude)
          end
        end
        return
      end
    end
  end
end

--- @class AUP.K
--- @field tmp_dir 'tmp_dir'

K.tmp_dir = 'tmp_dir'

--- The current directory for temporary material.
---
--- Create the directory as side effect.
--- @return string
function Units:tmp_dir_current()
  local ans = assert(self:state_get_string(K.tmp_dir))
  pl_dir.makepath(ans)
  return ans
end

--- Declares a failure.
--- @param message string
function Units:fail(message)
  print("FAILED: "..message.."\n")
  self._failures:append({
    test = self.test_current,
    message = message,
  })
end

-- Shared code
-- @param banner string
-- @param list pl.List
-- @return integer
local function print_result(banner, list)
  if list ~= nil and list:len() > 0 then
    print(banner..":")
    for x in list:iter() do
      local l = List()
      if x.test then
        l:append('test: '..x.test.path)
      end
      if x.message then
        l:append(x.message)
      end
      print("  "..l:concat(" — "))
    end
  end
  return list:len()
end

--- Print failures.
--- @return integer the number of failures
function Units:print_failed()
  return print_result("FAILED", self._failures)
end

--- @class AUP.Units
--- @field _skipures pl.List

--- Declares a skipped test.
--- @param message string
function Units:skip_one_test(message)
  self._skipures:append({
    test = self.test_current,
    message = message,
  })
end

--- Print failures.
--- @return integer the number of failures
function Units:print_skipped()
  return print_result("Skipped", self._skipures)
end


local Engine = AUP.Engine

--- Send this message when the engine must be of the given year.
---
--- @param when 'before'|'in'|'after'
--- @return boolean
function Units:only_engine(when)
  assert(self.test_year ~= nil, "`only_engine` not allowed at the top level.")
  if self.test_unit == nil then
    local year = Engine.year()
    if when == 'before' and year <= self.test_year
    or when == 'in' and year == self.test_year
    or when == 'after' and year >= self.test_year then
      self:test_foreach{
        action = function(t)
          t.excluded = true
          if not t.visited then
            self:skip_one_test('')
          end
        end,
        mode = self.test_mode,
        year = self.test_year,
        suite = self.test_suite
      }
    end
  end
  return false
end

return {
  _VERSION = '0.1',
  _DESCRIPTION = 'AUPLib unit test',
  Units = Units,
  Test = Test
}
