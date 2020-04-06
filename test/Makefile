TEST_DIR := $(shell pwd)
TEST_WRAPPER ?=

test: export LV2_PATH=$(TEST_DIR)/fixtures
test:
	$(TEST_WRAPPER) lua5.3 test_basic.lua --verbose

.PHONY: test
