-- Lua initialisation file for Vassal 4
--
--

--
--
math.randomseed(os.time())

-- The 'this' object of the currently running script or expression. This is a reference to the Game Object that
-- the script/expression is running against (e.g. a Piece or a Map)
_V_this = {}

-- A pointer to the context of the currently running script. The context is where the script engine keeps track of the links
-- between this script and Vassal Objects.
_V_context = {}

-- A stack of prior contexts when a new Lua script is called before the old one completes.
-- Each entry in the context stack is a table containing the 'this' object and the context ptr.
_V_contextStack = {}

-- A cache of compiled code for scripts so scripts do not need to be recompile when re-run by the same or different objects
_V_codeCache = {}


-- Constants to communicate execution status back to Vassal side
_V_SUCCESS = 0
_V_SCRIPT_ERROR = 1
_V_VASSAL_ERROR = 2

--- Call back into Vassal to perform some operation on a Vassal Game Piece Object.
--
-- Note user scripts cannot call _V_callback directly, it is not in their environment. Pre-made Lua Game Objects
-- compiled in the Global Environment are fed to the scripts on entry to the user script (through _M.eval) or
-- returned via Callbacks.
--
-- @param type The type of Game Object (see Scriptable.ScriptableType)
-- @param ptr a Lua light userdata pointing to the Vassal Game Object
-- @param op the operation to be performed on the Game Object
-- @param ... 0 or more arguments required to perform the operation
--
-- @return 1 result only, then r1 is a simple Lua data type
-- @return 2 results for a Vassal proxy type:
--            r1 = Lua lightuserdata pointing to the Vassal Game Object
--            r2 = Type of Vassal Game Object
--
_V_callback = function (type, ptr, op, ...)
  -- print ("_V_callback: type="..tostring(type)..", ptr="..tostring(ptr)..", op="..tostring(op));
  local r1, r2 =  callback (type, ptr, _V_context, op, ...)
  -- print ("_V_callback: return #1="..tostring(r1)..", r2="..tostring(r2)..", r3="..tostring(r3))
  if r2 == nil then
    return r1
  else
    return _V_NewObject (r2, r1)
  end
end

---------------------------------------------------------------------------------------------------------
-- The following section defines the Lua Object protypes that allow interaction with Vassal Game Objects.
---------------------------------------------------------------------------------------------------------

-- Create an empty table to use a Proxy for the real object table. Define a metatable with __index and__newindex
-- methods to re-direct reads and writes to the object appropriately.
-- Arguments
--   object     The Object to create the proxy for
--   objectName
function CreateObjectProxy (object, objectName)
  local proxy = {}   -- The Proxy table
  local proxymt = {} -- The Proxy metatable

  -- Catch attempts to write to the proxy. Writing is only allowed to entries that have a key in the __properties field
  -- of the target table (defined in the Base class for the object). The property name and value are passed to the
  -- set method of the appropriate proxy via the C++ callback
  proxymt.__newindex =
    function (t, n, v)
       if object.__properties[n] then
         _V_callback (object.type, object.ptr, 'set', n, v)
       else
         raiseError(object.__name..": Attempt to write undefined property "..tostring(n), _V_SCRIPT_ERROR, 2)
       end
    end

  -- Catch attempts to read from the proxy. Reads are re-directed in the following manner:
  -- 1. If the target exists in the real table, return it.
  -- 2. If the target exists as an entry in the __properties field of the target, call the get property of the target object to return the property value
  -- 3. Otherwise, assume the call is to an object method. Call the appropriate method of the target object
  proxymt.__index = function (t, n)
    if object[n] then
      return object[n]
    else
      if object.__properties[n] then
       return _V_callback (object.type, object.ptr, 'get', n)
      else
        return function (table, ...)
                 -- print ('In anonymous function')
                 return _V_callback (object.type, object.ptr, n, ...)
               end
      end
    end
  end
  setmetatable (proxy, proxymt)
  return proxy
end

--
-- Lua Piece Object. Allows Lua scripts to interact with a Vassal Piece Object.
--
local Piece = {}
Piece.__index = Piece
Piece.__name = "Piece"
Piece.__lock = true
Piece.__properties = {}

function Piece:new (type, ptr)
  --print('Lua:Piece:new Create new piece, type='..tostring(type)..', ptr='..tostring(ptr))
  local piece = {}
  setmetatable (piece, Piece)
  piece.type = type
  piece.ptr = ptr

  return CreateObjectProxy (piece, "Piece")
end

_V_Piece = Piece

--
-- Lua Map Object. Allows Lua scripts to interact with a Vassal Map Object.
--
local Map = {}
Map.__index = Map
Map.__name = "Map"
Map.__lock = true
Map.__properties = {}


function Map:new (type, ptr)
  -- print('Lua:Map:new, type='..tostring(type)..', ptr='..tostring(ptr))
  local map = {}
  setmetatable (map, Map)
  map.type = type
  map.ptr = ptr
  return CreateObjectProxy (map, "Map")
end


function Map:getVisiblePieces()
  -- print ('In Lua:Map:getVisiblePieces')
  local iter = _V_callback (self.type, self.ptr, "getVisiblePieces")
  return function ()
    local piece = _V_callback (self.type, self.ptr, "getVisiblePiecesNext")
    return piece
  end
end

_V_Map = Map

---------------------------------------------------------------------------------------------------------
-- Object table. Links the Scriptable::ScriptableType enums to Lua types
-- Thus table MUST match Scriptable::ScriptableType [Note that 0 in invalid for Scriptable type]
---------------------------------------------------------------------------------------------------------
_V_ObjectMap = {}
_V_ObjectMap[1] = _V_Piece
_V_ObjectMap[2] = _V_Map

---------------------------------------------------------------------------------------------------------
-- Create an appropriate Lua object based on a context type and pointer
---------------------------------------------------------------------------------------------------------
_V_NewObject = function (contextType, contextPtr)
--print ('In _V_NewObject, contextType='..tostring(contextType)..', contextPtr='..tostring(contextPtr))
  local o = _V_ObjectMap[contextType];
  return o:new(contextType, contextPtr)
end


------------------------------------ Sandbox Code ----------------------------------------------------

_M = {}
--- Small library for running Lua code in a sandbox.
--
-- @module sandbox
-- @author darkstalker <https://github.com/darkstalker>
-- @license MIT/X11
local assert, error, getmetatable, loadstring, pairs, pcall, select, setfenv, setmetatable, table_concat, type =
  assert, error, getmetatable, loadstring, pairs, pcall, select, setfenv, setmetatable, table.concat, type

local table_pack = table.pack or function(...) return { n = select("#", ...), ... } end
local table_unpack = table.unpack or unpack

-- TODO Support for Lua 5.1 not required - remove. Or leave in case we move to LuaJit?
local has_52_compatible_load = _VERSION ~= "Lua 5.1" or tostring(assert):match "builtin"
local load = has_52_compatible_load and load or function(code, name, mode, env)
  --mode = mode or "bt"
  if code:byte(1) == 27 --[[and not mode:match "b"]] then return nil, "can't load binary chunk" end
  local chunk, err = loadstring(code, name)
  if chunk and env then setfenv(chunk, env) end
  return chunk, err
end

local function pack_1(first, ...) return first, table_pack(...) end


-- Builds the environment table for a sandbox.
-- Copy entries that exist in the whitelist from the global environment table to the new table.
local function build_env(src_env, dest_env, whitelist)
  dest_env = dest_env or {}
  assert(getmetatable(dest_env) == nil, "env has a metatable")

  local env = {}
  for name in whitelist:gmatch "%S+" do
    local t_name, field = name:match "^([^%.]+)%.([^%.]+)$"
    if t_name then
      local tbl = env[t_name]
      local env_t = src_env[t_name]
      if tbl == nil and env_t then
        tbl = {}
        env[t_name] = tbl
      end
      if env_t then
        local t_tbl = type(tbl)
        if t_tbl ~= "table" then
          error("field '".. t_name .. "' already added as " .. t_tbl)
        end
        tbl[field] = env_t[field]
      end
    else
      local val = src_env[name]
      assert(type(val) ~= "table", "can't copy table reference")
      env[name] = val
    end
  end

  env._G = dest_env

  return env
end

--- List of safe library methods (5.1 to 5.3)
--- Modified for use by Vassal
--- - Removed coroutine.* entries
_M.whitelist = [[

_VERSION assert error ipairs next pairs pcall select tonumber tostring type unpack xpcall print

math.abs math.acos math.asin math.atan math.atan2 math.ceil math.cos math.cosh math.deg math.exp
math.floor math.fmod math.frexp math.huge math.ldexp math.log math.log10 math.max math.maxinteger
math.min math.mininteger math.mod math.modf math.pi math.pow math.rad math.random math.sin
math.sinh math.sqrt math.tan math.tanh math.tointeger math.type math.ult

os.clock os.difftime os.time os.date

string.byte string.char string.find string.format string.gmatch string.gsub string.len string.lower
string.match string.pack string.packsize string.rep string.reverse string.sub string.unpack
string.upper

table.concat table.insert table.maxn table.pack table.remove table.sort table.unpack

utf8.char utf8.charpattern utf8.codepoint utf8.codes utf8.len utf8.offset
]]

--- Entry point for Vassal to run a user supplied Script or Expression
--
-- @param code             Lua source code string.
-- @param name             Name of the chunk (for errors, default "sandbox").
-- @param contectPtr       A Lightuserdata (C Pointer) that is the address of the ContextFrame containing the links to Vassal for this invocation
-- @return                 The chunk return values, or an error message.
function _M.eval(code, name, thisType, thisPtr, contextPtr)

  -- print('In _M.eval, thisPtr='..tostring(thisPtr)..', contextPtr='..tostring(contextPtr))

  -- Parameter sanity test
  if (type(code) ~= "string") then
    raiseError ("Attempt to execute code that is not a string", _V_VASSAL_ERROR, 1)
  end

  if (_V_ObjectMap[thisType] == nil) then
    raiseError( "Invalid this type "..tostring(thisType), _V_VASSAL_ERROR, 1)
  end

  -- Reset the Vassal context to the one before this call
  local function popThis ()
    local context = _V_contextStack [#_V_contextStack]

    _V_context = context.contextPtr;
    _V_this = context.this;
    _V_env.this = _V_this

    table.remove (_V_contextStack);
  end

  -- Sandboxed version of rawget
  local function lock_rawget (t, n)
    local rawget = rawget
    if (t.__lock) then
      raiseError ("May not use rawget on a locked table", _V_SCRIPT_ERROR, 2)
    else
      local ok, result = pcall (rawget, t, n)
      if ok then
        return result
      else
        raiseError (result, _V_VASSAL_ERROR, 2)
      end
    end
  end

  -- Sandboxed version of rawset
  local function lock_rawset (t, n, v)
    local rawset = rawset
    if (t.__lock) then
      raiseError ("May not use rawset on a locked table", _V_SCRIPT_ERROR, 2)
    else
      local ok, result = pcall (rawset, t, n, v)
      if ok then
        return result
      else
        raiseError (result, _V_VASSAL_ERROR, 2)
      end
    end
  end

  -- Sandboxed version of getmetatable
  local function lock_getmetatable (t)
    local getmetatable = getmetatable
    if (t.__lock) then
      raiseError ("May not use getmetatable on a locked table", _V_SCRIPT_ERROR, 2)
    else
      local ok, result = pcall (getmetatable, t)
      if ok then
        return result
      else
        raiseError (result, _V_VASSAL_ERROR, 2)
      end
    end
  end

  -- Sandboxed version of setmetatable
  local function lock_setmetatable (t, mt)
    local setmetatable = setmetatable
    if (t.__lock) then
      raiseError ("May not use setmetatable on a locked table", _V_SCRIPT_ERROR, 2)
    else
      local ok, result = pcall (setmetatable, t, mt)
      if ok then
        return result
      else
        raiseError (result, _V_VASSAL_ERROR, 2)
      end
    end
  end

  -- Sandboxed version of string.rep to prevent abuse
  local function lock_string_rep (s, n, sep)
    local string = {rep = string.rep}
    if (type(n) == "number" and n > 40) then
      raiseError ("Bad argument #2 to 'rep' (too many copies)", _V_SCRIPT_ERROR, 2)
    else
      local ok, result = pcall (string.rep, s, n, sep)
      if ok then
        return result
      else
        raiseError (result, _V_VASSAL_ERROR, 2)
      end
    end
  end

  -- Sandboxed __index metamethod to make global environment table read only
  local function lock_newindex(t, n, v)
    --print ('in lock_newindex for '..tostring(n))
    raiseError("Attempting to set Global Variable '" .. tostring(n) .. "' to "..tostring(v)..".", _V_SCRIPT_ERROR, 2)
  end

  -- Sandboxed __newindex metamethod to warn prevent users trying to access undefined global environment entries
  local function lock_index(t, n)
    --print ('in lock_index for '..tostring(n))
    if _V_env[n] == nil then
      raiseError("Attempting to read undefined value '" .. tostring(n) .. "'.", _V_SCRIPT_ERROR, 2)
    end
    return _V_env[n]
  end

  if _V_env == nil then
    -- Create a limited copy of the current global environment
    _V_env = build_env (_G or _ENV, nil, _M.whitelist)

    -- Mark this environment as locked
    _V_env.__lock = true;

    --
    -- When creating sandboxed versions of individual functions, it is important that
    -- the correct function name and script line number is reported. To do this:
    -- 1. Call the local version of the sandboxed routine the same name as the original routine (eg. local rawget = rawget)
    -- 2. Execute the old routine using pcall and trap and report any error using error (message, 2)

    -- Add a sandboxed version of rawget

    _V_env.rawget = lock_rawget

    -- Add a sandboxed version of rawset
    _V_env.rawset = lock_rawset

    -- Add a sandboxed version of getmetatable
    _V_env.getmetatable = lock_getmetatable

    -- Add a sandboxed version of setmetatable
    _V_env.setmetatable = lock_setmetatable

    -- Create a metatable using the new locked __index and __newindex, but do not apply yet.
    _V_mt = { __index = lock_index, __newindex=lock_newindex}

  end

  -- The context of the Vassal object that calls this script is copied from _V_this and passed to the executing
  -- script as Global Variable this in the sandboxed environment passed to the script.
  -- Push any existing _V_this object onto a stack and replace it with a new one referencing the new context
  -- This happens when a second script or expression is called before an earlier one finishes.
  table.insert (_V_contextStack, {this=_V_this, context=_V_context})

  -- Build a new 'this' object based on the new context
  -- Save 'this' to _V_this in the global context
  -- Save 'this' to the sandboxed environment for user scripts to access the current Vassal Game Object
  -- Save the new context in the global environment
  _V_context = contextPtr
  _V_this = _V_NewObject(thisType, thisPtr)
  _V_env.this = _V_this

  -- Create a new empty environment for the script to run in.
  -- Set the metatable on the empty environment to redirect all reads and writes to our sandboxed environment
  local localEnvionment = {}
  setmetatable (localEnvionment, _V_mt)

  -- Cache compiled code keys on text so scripts do not need to be re-compiled each time they are run and the one compiled script
  -- can be shared by all Game Objects that use it.
  local fn = _V_codeCache[code]
  if fn == nil then
    local err
    -- Compile the code and return the error it fails
    fn, err = load(code, name or "sandbox", "t", localEnvionment)
    if fn == nil then
      popThis()
      raiseError (err, _V_SCRIPT_ERROR, 2, name)
    end
    _V_codeCache[code] = fn
  end

  -- Execute the sandboxed code and return error if there was a runtime error
  -- print('Enter sandbox')
  local ok, ret = pack_1(pcall(fn))
  -- print('Leave sandbox')
  --local ok, ret = 1, {"All fine here!"}

  -- Reset the context to what it was before this call.
  popThis()

  -- Error! return the error object to LuaEnvironment
  if not ok then
    -- print ('_M.eval: Function failed to run at context level '..(#_V_contextStack+1))
    -- print (tostring(ret[1].error))
    --raiseError ( ret[1], _V_SCRIPT_ERROR , 2, name)
    return ret[1], _V_SCRIPT_ERROR
  end

  -- print ('_M.eval: Function succeeded at context level '..(#_V_contextStack+1))

  -- Success! Return all results. Return an empty string result if there where no results.
  if (#ret == 0) then
    -- print ('_M.eval: Function succeeded, no return value, returning 1 args=empty string')
    return "", _V_SUCCESS
  else
    -- print ('_M.eval: Function succeeded, returning '..tostring(#ret)..' args')
    return ret[1], _V_SUCCESS
  end
end


------------------------------------ End Sandbox Code ----------------------------------------------------

-- End Lua initialisation
