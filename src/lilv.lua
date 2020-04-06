local _M = require('lilv.core')

--[[ local plugin_meta = getmetatable(_M.Plugin)
function plugin_meta:get(predicate)
  local v = self:get_value(predicate)
  if v ~= nil then return v[1] end
  return v
end

local port_meta = getmetatable(_M.Port)
function port_meta:get(predicate)
  local v = self:get_value(predicate)
  if v ~= nil then return v[1] end
  return v
end
 ]]

return _M