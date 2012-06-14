--
-- RemDebug for LuaIDE
-- Copyright Kepler Project 2005 (http://www.keplerproject.org/remdebug)
--
-- Modified 2011-2012 for LuaIDE project by Simon Mikuda to LimDebug
-- Bachelor project on STU Fiit
-- Project manager: Ing. Michal Kottman
-- Copyright Simon Mikuda STU Fiit 2012
--

local socket = require "socket"
local lfs = require "lfs"
local debug = require "debug"
local ml = require "ml"

module("limdebug.engine", package.seeall)

_COPYRIGHT = "2006 - Kepler Project, 2011 - Simon Mikuda"
_DESCRIPTION = "Remote Debugger for the Lua programming language\nModified to LimDebug"
_VERSION = "1.0"

-- Function taken from Microlight, Steve Donovan, 2012; License MIT
--- map a function over a array.
-- The output must always be the same length as the input, so
-- any `nil` values are mapped to `false`.
-- @param f a function of one or more arguments
-- @param t the array
-- @param ... any extra arguments to the function
-- @return a array with elements `f(t[i],...)`
function imap(f,t,...)
    f = ml.function_arg(f)
    local res = {}
    for i = 1,#t do
        res[i] = f(t[i],...) or false
    end
    return res
end


-- initialize init state of _G table
local base = {}
for k,v in pairs(_G) do
    base[k] = true
end

local coro_debugger
local local_vars = {}
local traceback
local events = { BREAK = 1, WATCH = 2 }
local breakpoints = {}
local watches = {}
local step_into = false
local step_over = false
local step_level = 0
local stack_level = 0

local controller_host = "localhost"
local controller_port = 8172

-- return global variables in table
function globals()
  local global_vars = {}
  for i, v in pairs(_G) do
    if not base[i] then
      global_vars[i] = v
    end
  end
  return global_vars
end

local function set_breakpoint(file, line)
  if not breakpoints[file] then
    breakpoints[file] = {}
  end
  breakpoints[file][line] = true
end

local function remove_breakpoint(file, line)
  if breakpoints[file] then
    breakpoints[file][line] = nil
  end
end

local function has_breakpoint(file, line)
  return breakpoints[file] and breakpoints[file][line]
end

local function restore_vars(vars)
  if type(vars) ~= 'table' then return end
  local func = debug.getinfo(3, "f").func
  local i = 1
  local written_vars = {}
  while true do
    local name = debug.getlocal(3, i)
    if not name then break end
    if vars[name] and not written_vars[name] then
      debug.setlocal(3, i, vars[name])
      written_vars[name] = true
    end
    i = i + 1
  end
  i = 1
  while true do
    local name = debug.getupvalue(func, i)
    if not name then break end
    if vars[name] and not written_vars[name] then
      debug.setupvalue(func, i, vars[name])
      written_vars[name] = true
    end
    i = i + 1
  end
end

local function capture_vars()
  local_vars = {}
  traceback = debug.traceback("", 4)
  local vars = {}
  local func = debug.getinfo(3, "f").func
  local i = 1
  while true do
    local name, value = debug.getupvalue(func, i)
    if not name then break end
    local_vars[name] = value
    vars[name] = value
    i = i + 1
  end
  i = 1
  while true do
    local name, value = debug.getlocal(3, i)
    if not name then break end
    -- ignore temporary variables
    if name:sub(1,1) ~= '(' then
      local_vars[name] = value
      vars[name] = value
    end
    i = i + 1
  end
  setmetatable(vars, { __index = getfenv(func), __newindex = getfenv(func) })
  return vars
end

local function break_dir(path)
  local paths = {}
  for w in string.gfind(path, "[^/]+") do
    table.insert(paths, w)
  end
  return paths
end

-- TODO use microlight functions instead of this
local function merge_paths(path1, path2)
  if path2:sub(1,1) == '/' then return path2 end
  local paths1 = break_dir(path1)
  local paths2 = break_dir(path2)
  for i, path in ipairs(paths2) do
    if path == ".." then
      table.remove(paths1, table.getn(paths1))
    elseif path ~= "." then
      table.insert(paths1, path)
    end
  end
  if path1:sub(1,1) == '/' then return "/" .. table.concat(paths1, "/")
  else return table.concat(paths1, "/") end
end

local function evalTable(tab)
	local numFields = 0
	local sertable = {}
	for i,v in pairs(tab) do
	    local val = tostring(v)
	    local key = tostring(i)
	    if type(i) == 'string' then key = '"' .. key .. '"' end
		sertable[#sertable+1] = tostring(string.len(key)) .. '\t'
		    .. tostring(string.len(val)) .. '\t'
			.. key .. '\t'
			.. type(v) .. '\t'
			.. val .. '\n'
		numFields = numFields + 1
	end
	return table.concat(sertable), numFields;
end

local function debug_hook(event, line)
  if event == "call" then
    stack_level = stack_level + 1
  elseif event == "return" or event == "tail return" then
    stack_level = stack_level - 1
  else
    -- get whole file path
	local file
	local dir = lfs.currentdir()
	if string.sub(dir, 1, 1) ~= '/'
	then -- widnows
		file = debug.getinfo(2, "S").short_src
		dir = string.gsub(dir, "\\", "/")
		file = string.gsub(file, "\\", "/")
		-- if file is on other partition, file got whole path
		if not string.find(file, ":") then
			file = merge_paths(dir, file)
		end
	else -- unix
		file = debug.getinfo(2, "S").source -- at beginning '@'
		file = merge_paths(dir, string.sub(file, 2))
	end
	-- print ("File: " .. file)
	-- print ("Dir: " .. dir)

    local vars = capture_vars()
    for index, value in ipairs(watches) do
      setfenv(value, vars)
      local status, res = pcall(value)
      if status and res then
        coroutine.resume(coro_debugger, events.WATCH, vars, file, line, index)
		restore_vars(vars)
      end
    end
    if step_into or (step_over and stack_level <= step_level) or has_breakpoint(file, line) then
      step_into = false
      step_over = false
      coroutine.resume(coro_debugger, events.BREAK, vars, file, line)
      restore_vars(vars)
    end
  end
end

local function debugger_loop(server)
  local command
  local eval_env = {}

  while true do
    local line, status = server:receive()
    command = string.sub(line, string.find(line, "^[A-Z]+"))
    --
    --	SETB command
    --
    if command == "SETB" then
      local _, _, filename, line = string.find(line, "^.... (.+) (%d+)$")
      if filename and line then
        filename = string.gsub(filename, "%%20", " ")
        set_breakpoint(filename, tonumber(line))
        server:send("200 OK\n")
      else
        server:send("400 Bad Request\n")
      end
    --
    --	DELB command
    --
    elseif command == "DELB" then
      local _, _, filename, line = string.find(line, "^.... (.+) (%d+)$")
      if filename and line then
        remove_breakpoint(filename, tonumber(line))
        server:send("200 OK\n")
      else
        server:send("400 Bad Request\n")
      end
    --
    --	EXEC command
    --
    elseif command == "EXEC" then
      local _, _, chunk = string.find(line, "^[A-Z]+%s+(.+)$")
      if chunk then
        local func = loadstring(chunk)
        local status, res
        if func then
          setfenv(func, eval_env)
          status, res = xpcall(func, debug.traceback)
        end
        if status then
          server:send("200 OK 0\n")
        else
          res = tostring(res)
          server:send("401 Error in Expression " .. string.len(res) .. "\n")
          server:send(res)
        end
      else
        server:send("400 Bad Request\n")
      end
    --
    --	EVAL command
    --
    elseif command == "EVAL" then
      local _, _, chunk = string.find(line, "^[A-Z]+%s+(.+)$")
      if chunk then
        local func = loadstring(chunk)
        local res, value
        if func then
          setfenv(func, eval_env)
          local function pack (...) return arg end
          res = pack(xpcall(func, debug.traceback))
        end
        -- first value is status
        if res[1] then
          -- there is 1 at the end of the table to indicate nil values at the end of table
          value = table.concat(imap(tostring, res), '\t', 2, #res - 1)
          server:send("200 OK " .. string.len(value) .. "\n")
          server:send(value)
        else
        -- send error message
          server:send("401 Error in Expression " .. string.len(res[2]) .. "\n")
          server:send(res[2])
        end
      else
        server:send("400 Bad Request\n")
      end
    --
    --	TABLE command
    --
    elseif command == "TABLE" then
      local _, _, chunk = string.find(line, "^[A-Z]+%s+(.+)$")
      if chunk then
        -- TODO set new environment for loadstring
        local func = loadstring('return ' .. chunk)
        local res, status, tab
        if func then
          setfenv(func, eval_env)
          status, tab = xpcall(func, debug.traceback)
        end
        if status and type(tab) == 'table' then
          res, tab = evalTable(tab)
          res = tostring(tab) .. '\n' .. res
          server:send("200 OK " .. string.len(res) .. "\n")
          server:send(res)
        else
        -- send error message
          res = ' "' .. chunk .. "\" is not a table"
          server:send("401 Error in Expression " .. string.len(res) .. "\n")
          server:send(res)
        end
      else
        server:send("400 Bad Request\n")
      end
    --
    --	LOCAL command
    --
    elseif command == "LOCAL" then
    	local locals, num = evalTable(local_vars)
    	local res = tostring(num) .. '\n' .. locals
		server:send("200 OK " .. string.len(res) .. "\n")
        server:send(res)
    --
    --	GLOBAL command
    --
    elseif command == "GLOBAL" then
    	local locals, num = evalTable(globals())
    	local res = tostring(num) .. '\n' .. locals
		server:send("200 OK " .. string.len(res) .. "\n")
        server:send(res)
    --
    --	TRACEBACK command
    --
    elseif command == "TRACEBACK" then
		server:send("200 OK " .. string.len(traceback) .. "\n")
		server:send(traceback)
    --
    --	SETW command
    --
    elseif command == "SETW" then
      local _, _, exp = string.find(line, "^[A-Z]+%s+(.+)$")
      if exp then
        local func = loadstring("return(" .. exp .. ")")
        watches[#watches + 1] = func
        if func then
        	server:send("200 OK " .. #watches .. "\n")
        else
        	server:send("401 Error in Expression\n")
        end
      else
        server:send("400 Bad Request\n")
      end
    --
    --	DELW command
    --
    elseif command == "DELW" then
      local _, _, index = string.find(line, "^[A-Z]+%s+(%d+)$")
      index = tonumber(index)
      if index then
        watches[index] = nil
        server:send("200 OK\n")
      else
        server:send("400 Bad Request\n")
      end
    --
    --	RUN command
    --
    elseif command == "RUN" then
      server:send("200 OK\n")
      local ev, vars, file, line, idx_watch = coroutine.yield()
      eval_env = vars
      if ev == events.BREAK then
        server:send("202 Paused " .. file .. " " .. line .. "\n")
      elseif ev == events.WATCH then
        server:send("203 Paused " .. file .. " " .. line .. " " .. idx_watch .. "\n")
      else
        server:send("401 Error in Execution " .. string.len(file) .. "\n")
        server:send(file)
      end
    elseif command == "STEP" then
      server:send("200 OK\n")
      step_into = true
      local ev, vars, file, line, idx_watch = coroutine.yield()
      eval_env = vars
      if ev == events.BREAK then
        server:send("202 Paused " .. file .. " " .. line .. "\n")
      elseif ev == events.WATCH then
        server:send("203 Paused " .. file .. " " .. line .. " " .. idx_watch .. "\n")
      else
        server:send("401 Error in Execution " .. string.len(file) .. "\n")
        server:send(file)
      end
    --
    --	OVER command
    --
    elseif command == "OVER" then
      server:send("200 OK\n")
      step_over = true
      step_level = stack_level
      local ev, vars, file, line, idx_watch = coroutine.yield()
      eval_env = vars
      if ev == events.BREAK then
        server:send("202 Paused " .. file .. " " .. line .. "\n")
      elseif ev == events.WATCH then
        server:send("203 Paused " .. file .. " " .. line .. " " .. idx_watch .. "\n")
      else
        server:send("401 Error in Execution " .. string.len(file) .. "\n")
        server:send(file)
      end
    else
      server:send("400 Bad Request\n")
    end
  end
end

coro_debugger = coroutine.create(debugger_loop)

--
-- remdebug.engine.config(tab)
-- Configures the engine
--
function config(tab)
  if tab.host then
    controller_host = tab.host
  end
  if tab.port then
    controller_port = tab.port
  end
end

--
-- remdebug.engine.start()
-- Tries to start the debug session by connecting with a controller
--
function start()
  pcall(require, "remdebug.config")
  local server = socket.connect(controller_host, controller_port)
  if server then
    _TRACEBACK = function (message)
      local err = debug.traceback(message)
      server:send("401 Error in Execution " .. string.len(err) .. "\n")
      server:send(err)
      server:close()
      return err
    end
    debug.sethook(debug_hook, "lcr")
    return coroutine.resume(coro_debugger, server)
  end
end

