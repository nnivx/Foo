if _name == 'main' then
    -- defined by program
    assert(_cwd, 'Expected _cwd')
else
    -- must be testing zb, remove in release
    local project_path = 'C:\\Users\\nyksn\\source\\repos\\Foo'
    _cwd = project_path
end

do
    local old_print<const> = print
    print = function(...)
        io.output():write('  ')
        old_print(...)
    end
end


-- Original package path.
local default<const> = package.path


-- Script paths for protected require switching.
local core, game, all
do
    local getsep = function () local t = {}; for w in package.config:gmatch '%g+' do t[#t + 1] = w end; return table.unpack(t) end
    
    local psep, tsep = getsep()
    
    local join = function (...) return table.concat({...}, psep) end
    
    local tcore_scripts = {
        join(_cwd, 'res', '.scripts', '?.lua'),
        join(_cwd, 'res', '.scripts', '?', 'init.lua'),
    }
    core = table.concat({default, table.unpack(tcore_scripts)}, tsep)
    
    local tgame_scripts = {
        join(_cwd, 'res', 'scripts', '?.lua'),
        join(_cwd, 'res', 'scripts', '?', 'init.lua'),
    }
    game = table.concat({default, table.unpack(tgame_scripts)}, tsep)
    
    all = table.concat({default, table.unpack(tcore_scripts), table.unpack(tgame_scripts)}, tsep)
end


-- Original require function.
local orig_require<const> = require


-- Protected require with custom path.
-- usage: prequire([core|game|all|default]) (module)
local function prequire(path)
    return function (modname)
        package.path = path or default
        -- use the original require for now until we can roll our own
        local re = { orig_require(modname) }
        package.path = default
        return table.unpack(re)
    end
end


-- Creates a new sandboxed environment.
local newenv
if _VERSION == 'Lua 5.4' then
    newenv = function(strict_require)
        local env = {
            assert = assert;
            -- collectgarbage = collectgarbage;
            -- dofile = dofile;
            error = error;
            -- getmetatable = getmetatable;
            ipairs = ipairs;
            -- load = load;
            -- loadfile = loadfile;
            next = next;
            pairs = pairs;
            pcall = pcall;
            print = print;
            -- rawequal = rawequal;
            -- rawget = rawget;
            -- rawlen = rawlen;
            -- rawset = rawset;
            select = select;
            -- setmetatable = setmetatable;
            tonumber = tonumber;
            tostring = tostring;
            type = type;
            _VERSION = _VERSION;
            -- warn = warn;
            xpcall = xpcall;
            require = strict_require;
        }
        
        -- somehow there was a subtle bug involving upvalues
        -- when involved with package.searchpath(name, path)
        -- so we cant use deepcopy here
        
        env.math = {}
        for k, v in pairs(math) do env.math[k] = v end
        
        env.string = {}
        for k, v in pairs(string) do env.string[k] = v end
        
        env.table = {}
        for k, v in pairs(table) do env.table[k] = v end
        
        env.coroutine = {}
        for k, v in pairs(coroutine) do env.coroutine[k] = v end
        
        env.utf8 = {}
        for k, v in pairs(utf8) do env.utf8[k] = v end
        
        return env
    end
end
if not newenv then
    return false, 'Cannot create sandboxed environment. Check your Lua version.'
end

---Sandboxed loadfile.
---usage: pload(core|game|all|default [, env]) (filename)
---@param path string path to file
---@param env table the env table
---@return function function that loads the given file
local function pload(path, env)
    return function (filename)
        local filename, e = package.searchpath(filename, path or default)
        
        if not filename then return filename, e end
        
        env = env or newenv(prequire(path))
        return loadfile(filename, 't', env)
    end
end


-- Sandboxed pcall.
-- usage: prun(core|game|all|default [, env]) (filename, args...)
local function prun(path, env)
    return function (filename, ...)
            local chunk, e = pload(path, env) (filename)
            if not chunk then return chunk, e end
            return pcall(chunk, ...)
        end 
end

local funcs = prequire(core) 'funcs'


do
    local test_script, err = prun(game) ('test_script')
    if not test_script then print(err) end
end

local events = {}
events.Key = 0
events.CharInput = 1
events.CursorEnter = 2
events.CursorPos = 3
events.MouseButton = 4
events.Scroll = 5
events.PathDrop = 6
events.FramebufferResize = 7
events.WindowClose = 8
events.WindowResize = 9
events.WindowMaximize = 10
events.WindowMinimize = 11
events.WindowFocus = 12


-- Event.h for event types
local function simple_event_listener(etype, timestamp, name, ...)
    print('scrolling ', select(1, ...), select(2, ...))
end

local function key_printer(etype, timestamp, name, ...)
    if select(3, ...) ~= 0 then
        print(etype, timestamp, name, ...)
    end
end

local function stepped_callback(etype, timestamp, name, ...)
    print('sleeping until resume...')
    coroutine.yield()
    print('awoken')
end

local co = coroutine.create(stepped_callback)
local yielding_wrapper = function(etype, timestamp, name, ...)
    if utf8.char(select(1, ...)) == 's' then
        if not coroutine.resume(co, etype, timestamp, name, ...) then 
            print 'we done broes sayonara'
            --api.remove_listener(that_thread)
        end
    end
end

    
that_thread = api.add_listener(yielding_wrapper, 1 << events.CharInput)

local e = api.add_listener(simple_event_listener, 1 << events.Scroll)
print (e)

api.add_listener(key_printer, 1 << events.Key)

