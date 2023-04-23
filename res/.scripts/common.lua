
    local deep_copy_table
    do
        local function naive_deep_copy_table(t)
            if type(t) ~= 'table' then return t end
            
            -- no cyclic checks, not tail recursive
            local copy = {}
            for k, v in pairs(t) do
                copy[k] = naive_deep_copy_table(v)
            end
            return copy
        end
        deep_copy_table = naive_deep_copy_table
    end