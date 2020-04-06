package.cpath = "../src/?.so;" .. package.cpath
package.path = "../src/?.lua;" .. package.path

T = require('luaunit')
lilv = require('lilv')

function shared_setup(self)
  self.w = lilv.World.new()
  self.w:load_all()
  self.plugins = self.w:get_all_plugins()
  self.plugin = self.plugins[1]
end

function shared_teardown(self)
  self.plugin = nil
  self.plugins = nil
  self.w = nil
  -- NOTE: this is not strictly needed but exercises memory mansgement which is not directly tested
  collectgarbage()
end

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
  T.assertIsTable(lilv.Port)
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
  T.assertEquals(#plugins, 2) --  Dummy, Synth
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
  shared_setup(self)
  self.class = self.plugin:get_class()
end

function TestPluginClass:tearDown()
  self.class = nil
  shared_teardown(self)
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

--
-- Plugin tests
--

TestPlugin = {}

function TestPlugin:setUp()
  shared_setup(self)
end

function TestPlugin:tearDown()
  shared_teardown(self)
end

function TestPlugin:test_verify()
  local ok = self.plugin:verify()
  T.assertTrue(ok)
end

function TestPlugin:test_get_uri()
  local uri = self.plugin:get_uri()
  T.assertNotNil(uri)
  T.assertEquals(uri:as_string(), "https://github.com/ngwese/lua-lilv/test/fixtures/dummy.lv2/Dummy")
end

function TestPlugin:test_get_bundle_uri()
  local uri = self.plugin:get_bundle_uri()
  local path = os.getenv("LV2_PATH") .. "/dummy.lv2/"
  local where = self.w:new_file_uri("", path)
  T.assertEquals(uri:as_string(), where:as_string())
end

function TestPlugin:test_get_value()
  local p = self.plugin
  -- ensure existing properties work
  local sym = self.w:new_uri("http://lv2plug.in/ns/lv2core#symbol")
  T.assertEquals(p:get_value(sym)[1]:as_string(), "Dmb")
  local minor = self.w:new_uri("http://lv2plug.in/ns/lv2core#minorVersion")
  T.assertEquals(p:get_value(minor)[1]:as_integer(), 2)
  local port = self.w:new_uri("http://lv2plug.in/ns/lv2core#port")
  local port_list = p:get_value(port)
  T.assertEquals(#port_list, 8)

  -- ensure missing property is nil
  local bogus = self.w:new_uri("http://lv2plug.in/ns/lv2core#bogus")
  T.assertIsNil(p:get_value(bogus))
end

function TestPlugin:test_get_name()
  local name = self.plugin:get_name()
  T.assertEquals(name, "dummy Dummy")
end

function TestPlugin:test_get_class()
  local class = self.plugin:get_class()
  -- NOTE: not sure how one discovers the more specific "ReverbPlugin" class
  T.assertEquals(class:get_uri():as_string(), "http://lv2plug.in/ns/lv2core#Plugin")
end

function TestPlugin:test_get_num_ports()
  T.assertEquals(self.plugin:get_num_ports(), 8)
end

function TestPlugin:test_get_port_by_index()
  -- ensure lua 1 base index converted to zero based index
  T.assertNil(self.plugin:get_port_by_index(0))
  T.assertIsUserdata(self.plugin:get_port_by_index(1))
  T.assertIsUserdata(self.plugin:get_port_by_index(8))
  T.assertNil(self.plugin:get_port_by_index(9))
end

function TestPlugin:test_get_port_by_symbol()
  -- ensure lua 1 base index converted to zero based index
  local missing = self.w:new_string("bogus_port_sym")
  local real = self.w:new_string("right_in")
  T.assertNil(self.plugin:get_port_by_symbol(missing))
  T.assertIsUserdata(self.plugin:get_port_by_symbol(real))
end

function TestPlugin:test_get_port_by_designation()
  -- verify lookup works
  local class_in = self.w:new_uri("http://lv2plug.in/ns/lv2core#InputPort")
  local class_out = self.w:new_uri("http://lv2plug.in/ns/lv2core#OutputPort")
  -- designation example `pg:left
  local desig = self.w:new_uri("http://lv2plug.in/ns/ext/port-groups#left")
  local p = self.plugin
  local left_in = p:get_port_by_designation(class_in, desig)
  T.assertIsUserdata(left_in)
  local left_out = p:get_port_by_designation(class_out, desig)
  T.assertIsUserdata(left_out)
  T.assertTrue(left_in ~= left_out)

  -- ensure failed lookup returns nil
  local bad_desig = self.w:new_uri("http://lv2plug.in/ns/ext/port-groups#bogus")
  T.assertNil(p:get_port_by_designation(class_in, bad_desig))
  local bad_class = self.w:new_uri("http://lv2plug.in/ns/lv2core#CarPort")
  T.assertNil(p:get_port_by_designation(bad_class, desig))
end

--
-- Port
--

TestPort = {}

function TestPort:setUp()
  shared_setup(self)
  self.mix_port = self.plugin:get_port_by_symbol(self.w:new_string("mix"))
  self.out_port = self.plugin:get_port_by_symbol(self.w:new_string("left_out"))

  self.synth = self.plugins[2]
  self.prop_port = self.synth:get_port_by_symbol(self.w:new_string("prop_example"))
end

function TestPort:tearDown()
  self.mix_port = nil
  self.out_port = nil
  self.synth = nil
  self.prop_port = nil
  shared_teardown(self)
end

function TestPort:test_get_value()
  local port = self.mix_port
  local sym = self.w:new_uri("http://lv2plug.in/ns/lv2core#symbol")
  T.assertEquals(port:get_value(sym)[1]:as_string(), "mix")
  -- ensure missing property is nil
  local bogus = self.w:new_uri("http://lv2plug.in/ns/lv2core#bogus")
  T.assertIsNil(port:get_value(bogus))
end

function TestPort:test_get_properties()
  -- port with properties
  local props = self.prop_port:get_properties()
  T.assertIsTable(props)
  T.assertEquals(#props, 2)

  -- port without properites
  local props = self.mix_port:get_properties()
  T.assertIsTable(props)
  T.assertEquals(#props, 0)
end

function TestPort:test_has_property()
  local expensive = self.w:new_uri("http://lv2plug.in/ns/ext/port-props/#expensive")
  T.assertTrue(self.prop_port:has_property(expensive))
  local strict = self.w:new_uri("http://lv2plug.in/ns/ext/port-props/#hasStrictBounds")
  T.assertFalse(self.prop_port:has_property(strict))
end

function TestPort:test_supports_event()
  local midi_port = self.synth:get_port_by_index(5)
  local event_type = self.w:new_uri("http://lv2plug.in/ns/ext/midi#MidiEvent")
  T.assertTrue(midi_port:supports_event(event_type))
  T.assertFalse(self.mix_port:supports_event(event_type))
end

function TestPort:test_get_symbol()
  local s = self.mix_port:get_symbol()
  T.assertIsUserdata(s)
  T.assertEquals(s:as_string(), "mix")
end

function TestPort:test_get_name()
  local n = self.out_port:get_name()
  T.assertIsString(n)
  T.assertEquals(n, "Left Out")
end

function TestPort:test_get_index()
  local idx = self.mix_port:get_index()
  T.assertEquals(idx, 3)
end

function TestPort:test_get_classes()
  local classes = self.mix_port:get_classes()
  T.assertIsTable(classes)
  T.assertEquals(#classes, 2)
  -- MAINT: not clear if the ordering is random or deterministic
  T.assertEquals(classes[1]:as_string(), "http://lv2plug.in/ns/lv2core#ControlPort")
  T.assertEquals(classes[2]:as_string(), "http://lv2plug.in/ns/lv2core#InputPort")
end

function TestPort:test_get_range()
  local r = self.mix_port:get_range()
  T.assertIsTable(r)
  T.assertEquals(#r, 3)  -- default, min, max
  T.assertAlmostEquals(r[1], 0.9, 0.00001) -- default
  T.assertAlmostEquals(r[2], 0.0) -- min
  T.assertAlmostEquals(r[3], 1.0) -- max
end


os.exit(T.LuaUnit.run())