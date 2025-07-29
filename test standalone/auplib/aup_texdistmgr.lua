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
local PL_class = PL.class
local PL_path = PL.path
local PL_List = PL.List
local PL_Map = PL.Map

local dbg = AUP.dbg

--- @alias AUPStringIter fun(): string?

--- The TeX distributions manager
--- @class AUPTeXDistMgr
--- @field singleton fun(): AUPTeXDistMgr
--- @field setup fun(self: AUPTeXDistMgr)
--- @field tex_bin_for_year fun(year: string|number): string
--- @field is_available fun(self: AUPTeXDistMgr, what: string|number): boolean
--- @field year fun(self: AUPTeXDistMgr, tex_bin: string): string?
--- @field tex_bin fun(self: AUPTeXDistMgr, year: string|number): string?
--- @field latest_tex_bin fun(self: AUPTeXDistMgr, year: string|number): string?
--- @field year_iter fun(self: AUPTeXDistMgr): AUPStringIter
--- @field year_reversed_iter fun(self: AUPTeXDistMgr): AUPStringIter
--- @field tex_bin_iter fun(self: AUPTeXDistMgr): AUPStringIter
--- @field tex_bin_reversed_iter fun(self: AUPTeXDistMgr): AUPStringIter
local AUPTeXDistMgr = PL_class()

--- Initialize an instance
--- @param self AUPTeXDistMgr
function AUPTeXDistMgr:_init()
  self.__setup = false
end

--- Get the location where official tex binaries are stored.
---
--- Static method.
--- The default implementation just raises because it must be overriden
--- by some local setup file. After this is defined in a configuration file,
--- The setup message must be sent to the manager.
--- @param year string|number
--- @return string
function AUPTeXDistMgr.tex_bin_for_year(year)
  error("`AUPTeXDistMgr.tex_bin_for_year` must be overriden in a `test_setup_local_⟨cfg⟩.lua` file")
end

---@alias AUPStringToString fun(string): string

--- Set the TeX disrtibution finder.
---
--- Must be called in a configuration file
--- (see test standalone/test_setup_unix_macosx.lua).
--- @param self AUPTeXDistMgr
--- @param f AUPStringToString
--- @return nil
function AUPTeXDistMgr:set_tex_bin_for_year(f)
  AUPTeXDistMgr.tex_bin_for_year = f
  self:setup()
end

--- Get the location where official tex binaries are stored.
---
--- The default implementation just raises because it must be overriden
--- by some local setup file. After this is defined in a configuration file,
--- The setup message must be sent to the manager.
--- @param self AUPTeXDistMgr
function AUPTeXDistMgr:setup()
---@diagnostic disable-next-line: inject-field
  self.__years = PL_List({})
---@diagnostic disable-next-line: inject-field
  self.__tex_bins = PL_List({})
---@diagnostic disable-next-line: inject-field
  self.__year_by_tex_bin = PL_Map({})
---@diagnostic disable-next-line: inject-field
  self.__tex_bin_by_year = PL_Map({})
for y = 2024, 2065 do
    local tex_bin = AUPTeXDistMgr.tex_bin_for_year(y)
    if tex_bin and PL_path.exists(tex_bin) then
      local year = tostring(y)
      self.__years:append(year)
      self.__tex_bins:append(tex_bin)
      self.__year_by_tex_bin:set(tex_bin, year)
      self.__tex_bin_by_year:set(year, tex_bin)
      self.__tex_bin_by_year:set(y, tex_bin)
    end
  end
  self.__years:sort('<')
  self.__tex_bins:sort('<')
---@diagnostic disable-next-line: inject-field
  self.__setup = true
  AUP.Command.tex_bin_setup()
end

--- Whether the distribution for the given argument is available
--- @param self AUPTeXDistMgr
--- @param what string|number a year or a path
function AUPTeXDistMgr:is_available(what)
  assert(self.__setup, "AUPTeXDistMgr.set_tex_bin_for_year(...) never executed.")
  return self.__tex_bin_by_year[what] ~= nil or self.__year_by_tex_bin[what] ~= nil
end


--- Return the year for the given path or nil if the path is not available
--- @param self AUPTeXDistMgr
--- @param tex_bin string a year or a path
function AUPTeXDistMgr:year(tex_bin)
  assert(self.__setup, "AUPTeXDistMgr.set_tex_bin_for_year(...) never executed.")
  return self.__year_by_tex_bin[tex_bin]
end


--- Return the tex_bin path for the given year or nil if the year is not available
--- @param self AUPTeXDistMgr
--- @param what string|number # a year or a path
function AUPTeXDistMgr:tex_bin(what)
  assert(self.__setup, "AUPTeXDistMgr.set_tex_bin_for_year(...) never executed.")
  return self.__tex_bin_by_year[what]
end

--- Return the year for the given path or nil if the path is not available
--- @param self AUPTeXDistMgr
--- @return AUPStringIter
function AUPTeXDistMgr:year_iter()
  assert(self.__setup, "AUPTeXDistMgr.set_tex_bin_for_year(...) never executed.")
  return self.__years:iter()
end


--- Return the year for the given path or nil if the path is not available
--- @param self AUPTeXDistMgr
--- @return AUPStringIter
function AUPTeXDistMgr:year_reversed_iter()
  assert(self.__setup, "AUPTeXDistMgr.set_tex_bin_for_year(...) never executed.")
  return self.__years:clone():reverse():iter()
end


--- Return the year for the given path or nil if the path is not available
--- @param self AUPTeXDistMgr
--- @return AUPStringIter
function AUPTeXDistMgr:tex_bin_iter()
  assert(self.__setup, "AUPTeXDistMgr.set_tex_bin_for_year(...) never executed.")
  return self.__tex_bins:iter()
end


--- Return the year for the given path or nil if the path is not available
--- @param self AUPTeXDistMgr
--- @return AUPStringIter
function AUPTeXDistMgr:tex_bin_reversed_iter()
  assert(self.__setup, "AUPTeXDistMgr.set_tex_bin_for_year(...) never executed.")
  return self.__tex_bins:clone():reverse():iter()
end


--- Return the latest tex_bin path for the given year
--- 
--- Raises when nothing is found
--- @param self AUPTeXDistMgr
--- @param what string|number # a year or a path
--- @return string
function AUPTeXDistMgr:latest_tex_bin(what)
  for year in self:year_reversed_iter() do
    local tex_bin = self:tex_bin(year)
    if tex_bin and year <= what then
      return tex_bin
    end
  end
  error("No TeX distribution available posterior to 2024.")
end


do
  local singleton = AUPTeXDistMgr()
--- Return the singleton instance
--- @return AUPTeXDistMgr
  AUPTeXDistMgr.singleton = function()
    return singleton
  end
end

--- @class AUP
--- @field TeXDistMgr AUPTeXDistMgr

AUP.TeXDistMgr = AUPTeXDistMgr

dbg:write(1, "aup_texdistmgr loaded")

return {
  TeXDistMgr = AUPTeXDistMgr
}
