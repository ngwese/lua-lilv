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
  self.w = lilv.World.new()
end

function TestNode:test_uri()
  local uri = "http://github.com/ngwese/lua-lilv"
  -- valid
  local a = self.w:new_uri(uri)
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertEquals(a:as_string(), uri)
  -- invalid
  local b = self.w:new_uri("bogus")
  T.assertNil(b)
end

function TestNode:test_file_uri()
  -- valid
  local a = self.w:new_file_uri("ahost", "relative")
  T.assertNotNil(a)
  T.assertIsUserdata(a)

  local b = self.w:new_file_uri("", "relative")
  T.assertNotNil(b)
  T.assertIsUserdata(b)

  local c = self.w:new_file_uri("ahost", "/absolute")
  T.assertNotNil(c)
  T.assertIsUserdata(c)
  T.assertEquals(c:as_string(), "file://ahost/absolute")

  local d = self.w:new_file_uri("", "/absolute")
  T.assertNotNil(d)
  T.assertIsUserdata(d)
  T.assertEquals(d:as_string(), "file:///absolute")
end

function TestNode:test_new_string()
  local a = self.w:new_string("foo")
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertEquals(a:as_string(), "foo")
end

function TestNode:test_new_int()
  local a = self.w:new_int(1234)
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertEquals(a:as_string(), "1234")
end

function TestNode:test_new_float()
  local a = self.w:new_float(1234.567)
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertStrContains(a:as_string(), "1234.567")
end

function TestNode:test_new_bool()
  local a = self.w:new_bool(true)
  T.assertNotNil(a)
  T.assertIsUserdata(a)
  T.assertEquals(a:as_string(), "1")
end

function TestNode:test_as_integer()
  local a = self.w:new_int(1234)
  T.assertEquals(a:as_integer(), 1234)
  local b = self.w:new_float(12.34)
  T.assertIsNil(b:as_integer())
end

function TestNode:test_as_number()
  local a = self.w:new_float(1.2)
  T.assertAlmostEquals(a:as_number(), 1.2, 0.000001)
  local b = self.w:new_int(1234)
  T.assertEquals(b:as_number(), 1234)
  local c = self.w:new_string("foo")
  T.assertIsNil(c:as_number())
end

function TestNode:test_equals()
  local a = self.w:new_string("foo")
  local b = self.w:new_string("foo")
  local c = self.w:new_string("notfoo")
  T.assertTrue(a == a)
  T.assertTrue(a == b)
  T.assertFalse(a == c)
end

function TestNode:test_duplicate()
  local a = self.w:new_int(3)
  T.assertNotNil(a)

  local b = a:duplicate()
  T.assertNotNil(b)

  T.assertTrue(a == b)
end

--
-- PluginClass tests
--

TestPluginClass = {}

function TestPluginClass:setUp()
  self.w = lilv.World.new()
  self.w:load_all()
  self.plugins = self.w:get_all_plugins()
  self.plugin = self.plugins[1]
  self.class = self.plugin:get_class()
end

function TestPluginClass:tearDown()
  self.class = nil
  self.plugin = nil
  self.plugins = nil
  self.w = nil
  -- NOTE: this is not strictly needed but excercises memory mansgement which is not directly tested
  collectgarbage()
end

function TestPluginClass:test_get_uri()
  local uri = self.class:get_uri()
  T.assertNotNil(uri)
  -- FIXME: why doesn't node equality work here
  -- T.assertEquals(uri, self.w:new_uri("http://lv2plug.in/ns/lv2core#Plugin"))
  T.assertEquals(uri:as_string(), "http://lv2plug.in/ns/lv2core#Plugin")
end

function TestPluginClass:test_get_parent_uri()
  local uri = self.class:get_parent_uri()
  -- FIXME: fixture needs class with parent
  --T.assertNotNil(uri)
  --print(uri)
end

function TestPluginClass:test_get_label()
  local label = self.class:get_label()
  T.assertNotNil(label)
  T.assertEquals(label, "Plugin")
end

function TestPluginClass:test_get_children()
  local children = self.class:get_children()
  T.assertNotNil(children)
  -- FIXME: fixture needs class with children
end


os.exit(T.LuaUnit.run())