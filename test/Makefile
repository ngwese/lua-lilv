TEST_DIR := $(shell pwd)

test: export LV2_PATH=$(TEST_DIR)/fixtures
test:
	lua5.3 test_basic.lua --verbose

.PHONY: test