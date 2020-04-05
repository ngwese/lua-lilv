package.cpath = "../src/?.so;" .. package.cpath
package.path = "../src/?.lua;" .. package.path

T = require('luaunit')
lilv = require('lilv')

--
-- Module test
--
function test_module_version()
  T.assertNotNil(lilv._VERSION)
  T.assertIsTable(lilv.World)
  T.assertIsTable(lilv.Node)
  T.assertIsTable(lilv.Plugin)
  T.assertIsTable(lilv.PluginClass)
end

--
-- World tests
--

function test_world_new()
  w = lilv.World.new()
  T.assertNotNil(w)
  T.assertIsUserdata(w)
end

function test_world_loading()
  w = lilv.World.new()
  w:load_all()
  plugins = w:get_all_plugins()
  T.assertNotNil(plugins)
  T.assertEquals(#plugins, 1)
end

--
-- Node tests
--

function test_node_uri()
  w = lilv.World.new()
  uri = "http://github.com/ngwese/lua-lilv"
  -- valid
  a = w:new_uri(uri)
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertEquals(a:as_string(), uri)
  -- invalid
  b = w:new_uri("bogus")
  T.assertNil(b)
end

function test_node_file_uri()
  w = lilv.World.new()
  -- valid
  a = w:new_file_uri("ahost", "relative")
  T.assertNotNil(a)
  T.assertIsUserdata(a)

  b = w:new_file_uri("", "relative")
  T.assertNotNil(b)
  T.assertIsUserdata(b)

  c = w:new_file_uri("ahost", "/absolute")
  T.assertNotNil(c)
  T.assertIsUserdata(c)
  T.assertEquals(c:as_string(), "file://ahost/absolute")

  d = w:new_file_uri("", "/absolute")
  T.assertNotNil(d)
  T.assertIsUserdata(d)
  T.assertEquals(d:as_string(), "file:///absolute")
end

function test_node_new_string()
  w = lilv.World.new()
  a = w:new_string("foo")
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertEquals(a:as_string(), "foo")
end

function test_node_new_int()
  w = lilv.World.new()
  a = w:new_int(1234)
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertEquals(a:as_string(), "1234")
end

function test_node_new_float()
  w = lilv.World.new()
  a = w:new_float(1234.567)
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertStrContains(a:as_string(), "1234.567")
end

function test_node_new_bool()
  w = lilv.World.new()
  a = w:new_bool(true)
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertEquals(a:as_string(), "1")
end

function test_node_as_integer()
  w = lilv.World.new()
  a = w:new_int(1234)
  T.assertEquals(a:as_integer(), 1234)
  b = w:new_float(12.34)
  T.assertIsNil(b:as_integer())
end

function test_node_as_number()
  w = lilv.World.new()
  a = w:new_float(1.2)
  T.assertAlmostEquals(a:as_number(), 1.2, 0.000001)
  b = w:new_int(1234)
  T.assertEquals(b:as_number(), 1234)
  c = w:new_string("foo")
  T.assertIsNil(c:as_number())
end

function test_node_equals()
  w = lilv.World.new()
  a = w:new_string("foo")
  b = w:new_string("foo")
  c = w:new_string("notfoo")

  T.assertTrue(a == a)
  T.assertTrue(a == b)
  T.assertFalse(a == c)
end

function test_node_duplicate()
  w = lilv.World.new()

  a = w:new_int(3)
  T.assertNotNil(a)

  b = a:duplicate()
  T.assertNotNil(b)

  T.assertTrue(a == b)
end


os.exit(T.LuaUnit.run())