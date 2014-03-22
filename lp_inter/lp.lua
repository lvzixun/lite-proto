local parse = require "parse"
local bit32 = require "bit32"

local extract = bit32.extract
local lshift = bit32.lshift
local bor    = bit32.bor

local mt = {}
mt.__index = mt

local function create()
  return setmetatable({block = {}}, mt)
end

function mt:dump_block()
  return table.concat(self.block)
end

function mt:write(str)
  return table.insert(self.block, str)
end

function mt:write_string(str)
  table.insert(self.block, str)
  table.insert(self.block, '\0')
end

function mt:write_byte(c)
  assert(c>=0 and c<= 0xff )
  return self:write(string.char(c))
end

local _ft2tag = {
  ["integer"] =  lshift(0, 3),
  ["real"]    =  lshift(1, 3),
  ["string"]  =  lshift(2, 3),
  ["bytes"]   =  lshift(3, 3),
}
function mt:write_tag(field_type, is_repeated)
  local r = 0
  if is_repeated then r = 1 end
  local ft = _ft2tag[field_type] or lshift(4, 3)
  local tag = bor(ft, r)
  self:write_byte(tag)
end

function mt:write_uint32(c)
  assert(c>=0 and c<= 0xffffffff)
  
  local b1, b2, b3, b4 = 
  extract(c, 0, 8),
  extract(c, 8, 8),
  extract(c, 16, 8),
  extract(c, 24, 8)

  self:write(string.char(b1, b2, b3, b4))
end

function mt:write_field(field_type, is_repeated, field_name)
  self:write_tag(field_type, is_repeated)
  if not _ft2tag[field_type] then
    self:write_byte(0)    --- fmes_internal @ lp_parse.c:6 the message type is defined at current file
    self:write_string(field_type)
  end
  self:write_string(field_name)
end

function mt:write_message(msg_name, fileds, msg_id)
  msg_id = msg_id or 0
  self:write_string(msg_name)
  self:write_uint32(msg_id)
  self:write_uint32(#fileds)
  for i=1,#fileds do
    local filed = fileds[i]
    self:write_field(filed.field_type, filed.is_repeated, filed.field_name)
  end
end


local function gen(s)
  local ast = parse(s)
  if type(ast) == "table" then
    local dump = create()
    for i=1, #ast do
      local msg = ast[i]
      dump:write_message(msg.msg_name, msg.value)
    end
    return true, dump:dump_block()
  end
  return false, ast
end


local function dump_file(file)
  local handle = io.open(file)
  local s = handle:read("*a")
  handle:close()

  local success, ret = gen(s)
  if success then
    file = file..".lpb"
    handle = io.open(file, "w")
    handle:write(ret)
    handle:close()
    print("dump file: "..file.." success.")
  else
    print(ret)
  end
end


local function exec(args)
  if #args == 0  or args[1]=="-h" then
    print[==[
    -h : help, default
    -o : parse .mes file to .mes.lpb exp: mesfile[ mesfile [...]]]==]
  elseif args[1] == "-o" then
    for i=2, #args do
      dump_file(args[i])
    end
  else
    print"invalid option, for \'-h \' detil"
  end
end 


exec({...})

---- test
-- dump_file("./testlp.mes")
