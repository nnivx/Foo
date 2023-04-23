print 'config.lua'

local argc, argv = ...
for k, v in ipairs(argv) do
    print(k, v)
end

local titles = {}
titles.May = {}
titles.May[25] = "Event System + Lua in One Day BABY"; 
titles.May[26] = {
    "Require and sandbox overhaul!",
    "require is plain messy ;;",
    'Bro I actually implemented the Event API too WTF'
}
titles.May[27] = 'Lots.. and lots of Lua internals';
titles.May[28] = {
    'Bug hunting, object lifetimes',
    'Overhauled Event and made subsystems',
    'ethreads!',
    --[[
        basically when an class holds a resource it automatically finalizes
        also owns an raii object that needs ^^that object, that resource
        should be a sibling and ordered in member declaration accordingly:
        
        // from
        class EnclosingClass {
            raii_object_needs_main sub;
            resource* main;
            ~EnclosingClass() { close(main); }
        };
        
        // to
        class resource_raii {
            resource* main;
            ~resource_raii() { close(main); }
        };
        
        class EnclosingClass {
            resource_raii main; //< order important
            raii_object_needs_main sub;
        };
        
        key takeaways
        - raii objects should probably be siblings
        - biggest enclosing class shouldn't be releasing resources
            manually, or if it has to should IN NO CASE be used or
            referred to whether directly or indirectly by downwards
            the composition heirarchy.
        - member order declaration matters LOL
    --]]
    'TIL make raii siblings if shared downwards',
}


-- Some dainty little function for easing stress and raising confidence
local function get_title()
    local flatten = require('funcs').flatten
    local t = {}
    local i = 0
    for k, v in pairs(flatten(titles)) do
        i = i + 1
        t[i] = string.format("%s - %s", k, v)
    end
    return t[math.random(#t)]
end

local title = argv[1] or get_title()
local w = 853
local h = 480

return {
    window_title = title;
    window_width = w;
    window_height = h;
    
    tps = 100;
}