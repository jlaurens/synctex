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
Formatted text with ascii codes.
THIS IS A WIP
--]==]

--- @class AUP
local AUP = package.loaded.AUP

local List = require"pl.List"
local pl_class = require"pl.class"

--- @class AUP.Ansi
local Ansi = pl_class()

AUP.Ansi = Ansi

--- @enum AUP.Ansi.Mode
Ansi.Mode = {
  Reset = '\033[0m',
  Bold = '\033[1m',
  Dim  = '\033[2m',
  Italic = '\033[3m',
  Underline = '\033[4m',
  BlinkSlow = '\033[5m',
  BlinkFast = '\033[6m',
  Reverse = '\033[7m',
  Hide = '\033[8m',
  StrikeThrough = '\033[9m'
}

--- @enum AUP.Ansi.Foreground
Ansi.Foreground = {
  Black = '\033[30m',
  Red = '\033[31m',
  Green = '\033[32m',
  Yellow = '\033[33m',
  Blue = '\033[34m',
  Purple = '\033[35m',
  Cyan = '\033[36m',
  Gray = '\033[37m'
}

--- @enum AUP.Ansi.Background
Ansi.Background = {
  Black = '\033[40m',
  Red = '\033[41m',
  Green = '\033[42m',
  Yellow = '\033[43m',
  Blue = '\033[44m',
  Purple = '\033[45m',
  Cyan = '\033[46m',
  Gray = '\033[47m'
}

---Apply some ANSI format to the given text
---
--- @param input string
--- @param ... unknown
--- @return string
function Ansi.format(input, ...)
  local l = List(arg)
  l:append(input)
  l:append(AUP.Ansi.Mode.Reset)
  return l:join('')
end

---Bold output
---
--- @param input string
--- @return string
function Ansi.bold(input)
  return Ansi.format(input, AUP.Ansi.Mode.Bold)
end

---Italic output
---
--- @param input string
--- @return string
function Ansi.italic(input)
  return Ansi.format(input, AUP.Ansi.Mode.Italic)
end

return {
  Ansi = AUP.Ansi
}