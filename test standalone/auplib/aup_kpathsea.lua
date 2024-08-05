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

-- This is not yet used

--- @class AUP
local AUP = package.loaded.AUP

local List     = require"pl.List"
local pl_utils = require"pl.utils"
local pl_class = require"pl.class"

local Command = AUP.Command

--- @class AUP.KPathSea: AUP.Command
--- @field super fun(self: AUP.KPathSea, name: string)
local KPathSea = pl_class(Command)

AUP.KPathSea = KPathSea

--- Initialize an AUP.KPathSea instance
function KPathSea:_init()
  self:super("KPathSea")
end

local quote_arg = pl_utils.quote_arg

--- Build the command on the fly.
--- @return string 
function KPathSea:cmd()
  local list = List({
    self._command,
  })
  list = list:filter(function(x)
    return type(x)=='string' and #x>0
  end)
  return quote_arg(list)
end

return {
  KPathSea = KPathSea,
}
