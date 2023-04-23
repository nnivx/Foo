
local ffi = require 'ffi'

-- makes for a nice syntax
local function child(root) 
    return function (id)
        return function (t)
            root[id] = t
        end
    end
end

local function screen(name)
    return function (t)
    end
end

root = {}

child(root) 'fancy' {
    value = true;
    
}

screen 'yes' {
    
}

print(root.fancy.value)