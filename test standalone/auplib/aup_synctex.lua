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

local List      = require"pl.List"
local pl_class  = require"pl.class"
local quote_arg = require"pl.utils".quote_arg

local Command = AUP.Command
local Result = AUP.Command.Result

-- The synctex command used may be located at different places.
-- When developing synctex, it is located in `.../meson/build`
-- When testing a distribution, it is located in the standard binary folder

---@class AUP.SyncTeX: AUP.Class
local SyncTeX = pl_class()

AUP.SyncTeX = SyncTeX

-- synctex -v
-- synctex --version
-- synctex --interactive this is meaningless
-- synctex --parse_int_policy C|raw1|raw2
--- @class AUP.SyncTeX.Global: AUP.Command
--- @field super fun(self: AUP.SyncTeX.Global, name: string)
local Global = pl_class(Command)

SyncTeX.Global = SyncTeX

---Initialize an instance
function Global:_init(name)
  print("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
  self:super(name or 'synctex_test')
  self._o = nil
  print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>")
end

--- Build the command on the fly
--- @return string errout
function Global:cmd()
  assert(self._command, "No engine available")
  assert(self._options, "No option available")
  local L = List{self._command}..self._options
  return quote_arg(L)
end

--- Run the command for the given global
--- @param option table|string
--- @param env table?
--- @return boolean status
--- @return AUP.Command.Result
function Global:run_option(option, env)
  self._options = type(option)=="table" and option or {option};
  return self:run(env)
end


-- synctex edit -o page:x:y:file [-d directory] [-x editor-command] [-h offset:context]
--- @class AUP.SyncTeX.Edit: AUP.Command
--- @field super fun(self: AUP.SyncTeX.Edit, name: string)
local Edit = pl_class(Command)

SyncTeX.Edit = Edit

---Initialize an instance
--- @param name string? defaults to `synctex`.
function Edit:_init(name)
  self:super(name or 'synctex_test')
end

---Set the `-o` option
--- @param page integer
--- @param x number
--- @param y number
--- @param file string
--- @return AUP.SyncTeX.Edit
function Edit:o(page, x, y, file)
  -- -o page:x:y:file
  self._o = '-o %i:%f:%f:%s' % {page, x, y, file}
  return self
end

---Set the `-d` option
--- @param directory string
--- @return AUP.SyncTeX.Edit
function Edit:d(directory)
  -- directory
  self._d = '-d %s' % {directory}
  return self
end

---Set the `-x` option
--- @param edit_command string
--- @return AUP.SyncTeX.Edit
function Edit:x(edit_command)
  -- -x edit-command
  self._x = '-x %s' % {edit_command}
  return self
end

---Set the `-h` option for the `edit` subcommand.
--- @param offset integer
--- @param context string
--- @return AUP.SyncTeX.Edit
function Edit:h(offset, context)
  -- -h offset:context
  self._h_edit = '-h %i:%s' % {offset, context}
  return self
end

---Reset the options.
--- @return AUP.SyncTeX.Edit
function Edit:reset()
  self._o = nil
  self._d = nil
  self._x = nil
  self._h = nil
  return self
end

--- Build the command on the fly
--- @return string errout
function Edit:cmd()
  assert(self._command)
  assert(self._o)
  local L = List({self._command, "edit", self._o}):extend(
    List({self._d or false, self._x or false, self._h or false}):filter(
      function(x)
        return type(x)=='string' and #x>0
      end
    )
  )
  return quote_arg(L)
end

-- synctex view -i line:column:[page_hint:]input -o output [-d directory] [-x viewer-command] [-h before/offset:middle/after]
--- @class AUP.SyncTeX.View: AUP.Command
--- @field super fun(self: AUP.SyncTeX.View, name: string?)
local View = pl_class(AUP.Command)

SyncTeX.View = View

---Initialize an instance
--- @param name string? defaults to `synctex`.
function View:_init(name)
  self:super(name or 'synctex_test')
end

---Set the `-i` optional
--- @param input string
--- @param line integer
--- @param column integer
--- @param page_hint integer?
--- @return AUP.SyncTeX.View
function View:i(input, line, column, page_hint)
  -- line:column:[page_hint:]input
  if type(page_hint) == 'nil' then
    self._i = '-i %i:%i:%s' % {line, column, input}
  else
    self._i = '-i %i:%i:%i:%s' % {line, column, page_hint, input}
  end
  return self
end

---Set the `-o` option
--- @param output string
--- @return AUP.SyncTeX.View
function View:o(output)
  -- output
  self._o = '-o %s' % {output}
  return self
end

---Set the `-d` option
--- @param directory string
--- @return AUP.SyncTeX.View
function View:d(directory)
  -- directory
  self._d = '-d %s' % {directory}
  return self
end

---Set the `-x` option
--- @param viewer_command string
--- @return AUP.SyncTeX.View
function View:x(viewer_command)
  -- -x viewer-command
  self._x = '-x %s' % {viewer_command}
  return self
end

---Set the `-h` option for the `view` subcommand.
--- @param offset integer
--- @param middle string
--- @param before string?
--- @param after string?
--- @return AUP.SyncTeX.View
function View:h_view(offset, middle, before, after)
  -- -h before/offset:middle/after
  before = before or ""
  after = after or ""
  self._h_view = '-h %s/%i:%s/%s' % {before, offset, middle, after}
  return self
end

---Reset the options.
function View:reset()
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
function View:cmd()
  assert(self._command)
  assert(self._i)
  assert(self._o)
  local L = List({self._command, "view", self._i, self._o}):extend(
    List({self._d or false, self._x or false, self._h or false}):filter(
      function(x)
        return type(x)=='string' and #x>0
      end
    )
  )
  return quote_arg(L)
end

-- synctex dump -o output [-d directory]
--- @class AUP.SyncTeX.Dump: AUP.Command
--- @field super fun(self: AUP.SyncTeX.Dump, name: string?)
local Dump = pl_class(Command)

SyncTeX.Dump = Dump

---Initialize an instance
--- @param name string? defaults to `synctex`.
function Dump:_init(name)
  self:super(name or 'synctex_test')
end

---Set the `-o` option
--- @param output string
--- @return AUP.SyncTeX.Dump
function Dump:o(output)
  -- output
  self._o = output
  return self
end

---Set the `-d` option
--- @param directory string
--- @return AUP.SyncTeX.Dump
function Dump:d(directory)
  -- directory
  self._d = directory
  return self
end

---Reset the options.
function Dump:reset()
  self._o = nil
  self._d = nil
  return self
end

--- Build the command on the fly
--- @return string command
function Dump:cmd()
  assert(self._command)
  assert(self._o)
  local L = List({self._command, "dump", "-o", self._o})
  if self._d then
    L:append("-d"):append(self._d)
  end
  return quote_arg(L)
end

return {
  SyncTeX = SyncTeX
}
