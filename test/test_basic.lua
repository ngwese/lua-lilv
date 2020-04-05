package.cpath = "../src/?.so;" .. package.cpath
package.path = "../src/?.lua;" .. package.path

T = require('luaunit')
lilv = require('lilv')

--
-- Module test
--

TestModule = {}

function TestModule:test_exports()
  T.assertNotNil(lilv._VERSION)
  T.assertIsTable(lilv.World)
  T.assertIsTable(lilv.Node)
  T.assertIsTable(lilv.Plugin)
  T.assertIsTable(lilv.PluginClass)
end

--
-- World tests
--

TestWorld = {}

function TestWorld:test_new()
  local w = lilv.World.new()
  T.assertNotNil(w)
  T.assertIsUserdata(w)
end

function TestWorld:test_loading()
  local w = lilv.World.new()
  w:load_all()
  plugins = w:get_all_plugins()
  T.assertNotNil(plugins)
  T.assertEquals(#plugins, 1)
end

--
-- Node tests
--

TestNode = {}

function TestNode:setUp()
  w = lilv.World.new()
end

function TestNode:test_uri()
  uri = "http://github.com/ngwese/lua-lilv"
  -- valid
  local a = w:new_uri(uri)
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertEquals(a:as_string(), uri)
  -- invalid
  local b = w:new_uri("bogus")
  T.assertNil(b)
end

function TestNode:test_file_uri()
  -- valid
  local a = w:new_file_uri("ahost", "relative")
  T.assertNotNil(a)
  T.assertIsUserdata(a)

  local b = w:new_file_uri("", "relative")
  T.assertNotNil(b)
  T.assertIsUserdata(b)

  local c = w:new_file_uri("ahost", "/absolute")
  T.assertNotNil(c)
  T.assertIsUserdata(c)
  T.assertEquals(c:as_string(), "file://ahost/absolute")

  local d = w:new_file_uri("", "/absolute")
  T.assertNotNil(d)
  T.assertIsUserdata(d)
  T.assertEquals(d:as_string(), "file:///absolute")
end

function TestNode:test_new_string()
  local a = w:new_string("foo")
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertEquals(a:as_string(), "foo")
end

function TestNode:test_new_int()
  local a = w:new_int(1234)
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertEquals(a:as_string(), "1234")
end

function TestNode:test_new_float()
  local a = w:new_float(1234.567)
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertStrContains(a:as_string(), "1234.567")
end

function TestNode:test_new_bool()
  local a = w:new_bool(true)
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertEquals(a:as_string(), "1")
end

function TestNode:test_as_integer()
  local a = w:new_int(1234)
  T.assertEquals(a:as_integer(), 1234)
  local b = w:new_float(12.34)
  T.assertIsNil(b:as_integer())
end

function TestNode:test_as_number()
  local a = w:new_float(1.2)
  T.assertAlmostEquals(a:as_number(), 1.2, 0.000001)
  local b = w:new_int(1234)
  T.assertEquals(b:as_number(), 1234)
  local c = w:new_string("foo")
  T.assertIsNil(c:as_number())
end

function TestNode:test_equals()
  local a = w:new_string("foo")
  local b = w:new_string("foo")
  local c = w:new_string("notfoo")
  T.assertTrue(a == a)
  T.assertTrue(a == b)
  T.assertFalse(a == c)
end

function TestNode:test_duplicate()
  local a = w:new_int(3)
  T.assertNotNil(a)

  local b = a:duplicate()
  T.assertNotNil(b)

  T.assertTrue(a == b)
end


os.exit(T.LuaUnit.run())