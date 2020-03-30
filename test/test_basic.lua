local path = "src/lilv.so"
local core = package.loadlib(path, "luaopen_lilv_core")
local lilv = core()

print(core)
print(lilv)
for k,v in pairs(lilv) do
  print(k, v)
end
