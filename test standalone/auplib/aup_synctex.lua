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

local AUPCommand = AUP.module.command

-- synctex edit -o page:x:y:file [-d directory] [-x editor-command] [-h offset:context]
--- @class AUPSyncTeXEdit: AUPCommand
--- @field _init fun(self: AUPSyncTeXEdit)
--- @field o fun(self: AUPSyncTeXEdit, page: integer, x: number, y: number, file: string): AUPSyncTeXEdit
--- @field d fun(self: AUPSyncTeXEdit): AUPSyncTeXEdit
--- @field x fun(self: AUPSyncTeXEdit): AUPSyncTeXEdit
--- @field h fun(self: AUPSyncTeXEdit, offset: integer, middle: string, before:string?, after: string?): AUPSyncTeXEdit
--- @field run fun(self: AUPSyncTeXEdit): boolean, integer, string, string
--- @field reset fun(self: AUPSyncTeXEdit)

local AUPSyncTeXEdit = PL.class.AUPSyncTeXEdit(AUPCommand)

---Initialize an instance
---@param name string? defaults to `synctex`.
function AUPSyncTeXEdit:_init(name)
  self:super(name or 'synctex_test')
end

---Set the `-o` option
---@param page integer
---@param x number
---@param y number
---@param file string
---@return function AUPSyncTeXEdit:o(page, x, y, file)
function AUPSyncTeXEdit:o(page, x, y, file)
  -- -o page:x:y:file
  self._o = '-o %i:%f:%f:%s' % {page, x, y, file}
  return self
end

---Set the `-d` option
---@param directory string
---@return AUPSyncTeXEdit
function AUPSyncTeXEdit:d(directory)
  -- directory
  self._d = '-d %s' % {directory}
  return self
end

---Set the `-x` option
---@param edit_command string
---@return AUPSyncTeXEdit
function AUPSyncTeXEdit:x(edit_command)
  -- -x edit-command
  self._x = '-x %s' % {edit_command}
  return self
end

---Set the `-h` option for the `edit` subcommand.
---@param offset integer
---@param context string
---@return AUPSyncTeXEdit
function AUPSyncTeXEdit:h(offset, context)
  -- -h offset:context
  self._h_edit = '-h %i:%s' % {offset, context}
  return self
end

---Reset the options.
---@return AUPSyncTeXEdit
function AUPSyncTeXEdit:reset()
  self._o = nil
  self._d = nil
  self._x = nil
  self._h = nil
  return self
end

local quote_arg = PL.utils.quote_arg

--- Build the command on the fly
--- @return string errout
function AUPSyncTeXEdit:cmd()
  assert(self._engine)
  assert(self._o)
  local L = List({self._engine, "edit", self._o}):extend(
    List({self._d, self._x, self._h}):filter(
      function(x)
        return type(x)=='string' and #x>0
      end
    )
  )
  return quote_arg(L)
end

-- synctex view -i line:column:[page_hint:]input -o output [-d directory] [-x viewer-command] [-h before/offset:middle/after]
--- @class AUPSyncTeXView: AUPCommand
--- @field _init fun(self: AUPSyncTeXView, name: string?)
--- @field i fun(self: AUPSyncTeXView, input: string, line: integer, column: integer, page_int: integer?): AUPSyncTeXView
--- @field o fun(self: AUPSyncTeXView): AUPSyncTeXView
--- @field d fun(self: AUPSyncTeXView): AUPSyncTeXView
--- @field x fun(self: AUPSyncTeXView): AUPSyncTeXView
--- @field h fun(self: AUPSyncTeXView, offset: integer, middle: string, before:string?, after: string?): AUPSyncTeXView
--- @field reset fun(self: AUPSyncTeXView)

local AUPSyncTeXView = PL.class.AUPSyncTeXView(AUPCommand)

---Initialize an instance
---@param name string? defaults to `synctex`.
function AUPSyncTeXView:_init(name)
  self:super(name or 'synctex_test')
end

---Set the `-i` optional
---@param input string
---@param line integer
---@param column integer
---@param page_hint integer?
---@return AUPSyncTeXView
function AUPSyncTeXView:i(input, line, column, page_hint)
  -- line:column:[page_hint:]input
  if type(page_hint) == 'nil' then
    self._i = '-i %i:%i:%s' % {line, column, input}
  else
    self._i = '-i %i:%i:%i:%s' % {line, column, page_hint, input}
  end
  return self
end

---Set the `-o` option
---@param output string
---@return AUPSyncTeXView
function AUPSyncTeXView:o(output)
  -- output
  self._o = '-o %s' % {output}
  return self
end

---Set the `-d` option
---@param directory string
---@return AUPSyncTeXView
function AUPSyncTeXView:d(directory)
  -- directory
  self._d = '-d %s' % {directory}
  return self
end

---Set the `-x` option
---@param viewer_command string
---@return AUPSyncTeXView
function AUPSyncTeXView:x(viewer_command)
  -- -x viewer-command
  self._x = '-x %s' % {viewer_command}
  return self
end

---Set the `-h` option for the `view` subcommand.
---@param offset integer
---@param middle string
---@param before string?
---@param after string?
---@return AUPSyncTeXView
function AUPSyncTeXView:h_view(offset, middle, before, after)
  -- -h before/offset:middle/after
  before = before or ""
  after = after or ""
  self._h_view = '-h %s/%i:%s/%s' % {before, offset, middle, after}
  return self
end

---Reset the options.
function AUPSyncTeXView:reset()
  -- -h before/offset:middle/after
  self._i = nil
  self._o = nil
  self._d = nil
  self._x = nil
  self._h = nil
  return self
end

--- Build the command on the fly
--- @return string errout
function AUPSyncTeXView:cmd()
  assert(self._engine)
  assert(self._i)
  assert(self._o)
  local L = List({self._engine, "view", self._i, self._o}):extend(
    List({self._d, self._x, self._h}):filter(
      function(x)
        return type(x)=='string' and #x>0
      end
    )
  )
  return quote_arg(L)
end

-- synctex dump -o output [-d directory]
--- @class AUPSyncTeXDump: AUPCommand
--- @field _init fun(self: AUPSyncTeXDump, name: string?)
--- @field o fun(self: AUPSyncTeXDump): AUPSyncTeXDump
--- @field d fun(self: AUPSyncTeXDump): AUPSyncTeXDump
--- @field reset fun(self: AUPSyncTeXDump)

local AUPSyncTeXDump = PL.class.AUPSyncTeXDump(AUPCommand)

---Initialize an instance
---@param name string? defaults to `synctex`.
function AUPSyncTeXDump:_init(name)
  self:super(name or 'synctex_test')
end

---Set the `-o` option
---@param output string
---@return AUPSyncTeXDump
function AUPSyncTeXDump:o(output)
  -- output
  self._o = output
  return self
end

---Set the `-d` option
---@param directory string
---@return AUPSyncTeXDump
function AUPSyncTeXDump:d(directory)
  -- directory
  self._d = directory
  return self
end

---Reset the options.
function AUPSyncTeXDump:reset()
  self._o = nil
  self._d = nil
  return self
end

--- Build the command on the fly
--- @return string command
function AUPSyncTeXDump:cmd()
  assert(self._engine)
  assert(self._o)
  local L = List({self._engine, "dump", "-o", self._o})
  if self._d then
    L:append("-d", self._d)
  end
  return quote_arg(L)
end

--- @class AUPSyncTeX
--- @field View AUPSyncTeXView
--- @field Edit AUPSyncTeXEdit
--- @field Dump AUPSyncTeXDump

AUP.SyncTeX = {
  View = AUPSyncTeXView,
  Edit = AUPSyncTeXEdit,
  Dump = AUPSyncTeXDump
}

return AUP.SyncTeX
