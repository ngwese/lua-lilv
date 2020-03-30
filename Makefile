# lua_lilv
#
# see src/Makefile for additional details
#
# Targets:
#   install            install system independent support
#   install-play       install platform specific support
#   print	           print the build settings

PLAT ?= macos
PLATS = macos linux

all: $(PLAT)

$(PLATS) none install install-plat local clean:
	$(MAKE) -C src $@

print:
	$(MAKE) -C src $@

test:
	lua test/test_basic.lua

.PHONY: test
