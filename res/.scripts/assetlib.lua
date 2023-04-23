-- mapping of asset id's and their actual filenames

local assets = {}

local getsep = function () local t = {}; for w in package.config:gmatch '%g+' do t[#t + 1] = w end; return table.unpack(t) end
    
local psep, tsep = getsep()

local function res(parent)
    return function(child) return table.concat({parent, child}, psep) end
end

assets.fonts = {
    ['notosans'] = res 'fonts' 'notosans.fnt';
    
}

local t = {1,2,3}
t[0] = 9
print(#t)
print(assets.fonts.notosans)