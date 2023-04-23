print 'back in .scripts'

function absi(t, i)
    return i < 0 and (#t + i + 1) or i
end

--[[
    Boot sequence
    
    glfw
    
    read args
        pre: none
    
    window
        pre: config
        post:
        
    glcontext
    
    game
    : lua
    : events
    : script
    : etc
    : resource handler
        - initialize
        
    main loop
        - update
        - integrate
        - blend
    
    game
        - cleanup


--]]
local api = require 'api'

do
    -- parse args here
    -- establish cwd, paths etc
end


do
    -- open config here
end


-- create new state
local foo = api.new()


-- initialize engine
foo:Initialize()

-- create window
local window, err = foo:CreateWindow {
    width   = 800;
    height  = 400;
    title   = "yoh";

    resizable = true;
}

do
    -- create game state
end

while not foo:ShouldClose() do
    
end

foo:Cleanup()

api.delete(foo)

