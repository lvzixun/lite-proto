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
      line = cur_line,
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
        msg_name = name,
        line = cur_line,
        value = {...}
      }
    end
}


local G = pass * Ct((M * pass)^0)


----- generating ast

local mt = {
  internal = {
    ["integer"] = true,
    ["string"] = true,
    ["real"] = true,
    ["bytes"] = true,
  }
}
mt.__index = mt

local function create()
  local obj = {
    msg_sym = {},
    msg_chain = {}
  }

  return setmetatable(obj, mt)
end

function mt:gen_ast(s)
  cur_line = 1

  local success, ret = pcall(
    function ()
      return lpeg.match(G, s)
    end)

  return ret
end

function mt:gen_fix(ast)
  local success, ret = pcall(
    function ()
      self:fix_ast(ast)
    end)

  if success then
    return self.msg_chain
  else
    return ret
  end
end

local function add_field(ret, v)
  assert(v.type == "field")
  v.cur_line = nil
  v.type = nil
  table.insert(ret.value, v)
end


function mt:add_msg(ret)
  return table.insert(self.msg_chain, ret)
end

function mt:fix_msg(ast, root)
  assert(ast.type == "message")

  local msg_name = ast.msg_name
  if root then
    msg_name = root.."."..msg_name
  end

  if self.msg_sym[msg_name] then
    error(("[error line: %d] redefine message \'%s\'"):format(ast.line, ast.msg_name))
  end

  self.msg_sym[msg_name] = true

  local ret = {
    msg_name = msg_name,
    value = {}
  }

  local value = ast.value

  for i=1, #value do
    local v = value[i]
    if v.type == "field" then
      if self.internal[v.field_type] then           -- internal type
        add_field(ret, v)
      elseif self.msg_sym[v.field_type] then        -- global message type
        add_field(ret, v)
      else                                       
        local ft = msg_name.."."..v.field_type
        if self.msg_sym[ft] then                    -- current message type
          v.field_type = ft
          add_field(ret, v)
        else
          error(("[error line: %d] invaild message type \'%s\' "):format(v.line, v.field_type))
        end
      end
    elseif v.type == "message" then
      self:fix_msg(v, msg_name)
    else
      assert(false)
    end
  end

  self:add_msg(ret)
end


function mt:fix_ast(ast)
  for i=1,#ast do
    local msg = ast[i]
    self:fix_msg(msg)
  end
end


local function gen(s)
  local parse = create()
  local ast = parse:gen_ast(s)
  if type(ast) == "table" then
    local ret = parse:gen_fix(ast)
    return ret
  else
    return ast
  end
end


return gen

-- -- test
-- local t_str = [[
-- message msg1 {
--     integer a1;
--     string a2;
--     message msg2 {
--       real a4;
--       message msg5 {
--       }
--     }
--     msg2  a5;
--     msg2.msg5 a8;
--     msg1.msg2 a9;
--     msg1.msg2.msg a10;
-- }

-- message msg4 {
--   real a6;
--   msg1.msg2 a7;
-- }
-- ]]

-- local ret = gen(t_str)

-- if type(ret) == "table" then
--   print(util.print_table(ret))
-- else
--   print(ret)
-- end



