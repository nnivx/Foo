local flatten
do
    -- I did this yeaaars ago now idk how the fuck it works but okay - me, 2022
    local function _flatten(r, pr, ks, it, t, k, v)
        if v == nil then
            -- we're done
            if #ks == 0 then return r end
            -- unpack saved state
            it, t, k, v = table.unpack(ks[#ks])
            ks[#ks] = nil
            pr[#pr] = nil
        else
            -- push the key as a prefix
            pr[#pr+1] = k
            if type(v) ~= 'table' then
                -- if it isnt a table, add to our results
                r[table.concat(pr, '.')] = tostring(v)
                pr[#pr] = nil
            else
                -- otherwise if it's a table save state
                ks[#ks + 1] = {it, t, k, v}
                it, t, k = pairs(v)
            end
        end
        return _flatten(r, pr, ks, it, t, it(t, k))
    end

    flatten = function(root)
        local it, t, v = pairs(root)
        return _flatten({}, {}, {}, it, t, it(t, v))
    end

end

return {
    flatten = flatten;
}