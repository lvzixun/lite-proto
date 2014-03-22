local util = require "util"
local lpeg = require "lpeg"

local tostring = tostring
local tonumber = tonumber
local pcall = pcall

local R, P, S, V, C, Ct, Cg, Cp, Cmt = 
  lpeg.R, lpeg.P, lpeg.S, lpeg.V, lpeg.C, lpeg.Ct, lpeg.Cg, lpeg.Cp, lpeg.Cmt

local cur_line = 1

--------------------- lexical  ---------------------
local I = Cp()
local space = S(" \r\t")
local line = P"\n" / 
  function ()
    cur_line = cur_line + 1
  end

local note = P("#")*(P(1)-line)^0
local pass = (space + line + note)^0

local _dec = R("09")^1
local _hex = (P("0x") + P("0X")) * (R("09", "AF", "af")^1)
local number = C(_hex + _dec) / tonumber

local _char = R("az", "AZ") + P("_")
local name = C(_char*(_dec + _char)^0)

-----  syntax ------- 
local internal_type = C(P"integer" + P"string" + P"real" + P"bytes")
local message_type = name * (P"." + name)^0 / 
  function (...)
    return table.concat({...}, ".")
  end

local repeated_type = (internal_type + message_type) * pass * P("[]") / 
  function (...)
    return ..., true
  end

internal_type = internal_type / 
  function (...)
    return ..., false
  end

message_type = message_type / 
  function (...)
    return ..., false
  end

local field = (repeated_type + internal_type + message_type) * pass * name * pass * P";" / 
  function (type, is_repeated, name)
    return {
      type = "field",
      is_repeated = is_repeated,
      field_type = type,
      field_name = name
    }
  end

local function exception(pos)
  error(("[error line: %d @%d] not expect token."):format(cur_line, pos))
end
exception = I*P(1) / exception


local msg, head, body = V"msg", V"head", V"body"
local M = P{
  "msg",
  head = P"message" * pass * name * pass * P"{" * pass,
  body = P"}" + (field + msg + exception) * pass * body,
  msg  = (head * body) / 
    function (name, ...)
      return {
        type = "message",
        value = {...}
      }
    end
}



local G = pass * Ct((M * pass)^0)

local function gen(s)
  cur_line = 1

  local success, ret = pcall(
    function ()
      return lpeg.match(G, s)
    end)

  if success then
    assert(type(ret) == "table")
    return ret
  else
    assert(type(ret) == "string")
    return ret
  end
end


return gen


---- test
-- local t_str = [[
-- message msg1 {
--     integer a1;
--     string a2;
--     a.b.c  a3;
--     message msg2 {
--       real a4;
--     }

--     msg2  a5;
-- }

-- message msg4 {
--   real a6;
-- }
-- ]]

-- local ret = gen(t_str)

-- if type(ret) == "table" then
--   print(util.print_table(ret))
-- else
--   print(ret)
-- end



