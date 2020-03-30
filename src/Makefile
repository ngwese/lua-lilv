# luaLILV src/makefile
#
# Definitions in this section can be overriden on the command line or in the
# environment.
#
# These are equivalent:
#
#    export PLAT=linux DEBUG=DEBUG LUAV=5.2 prefix=/sw
#    make
#
# and
#
#   make PLAT=linux DEBUG=DEBUG LUAV=5.2 prefix=/sw

# PLAT: linux macos
# platform to build for
PLAT ?= linux

# LUAV: 5.1 5.2
# lua version to build against
LUAV ?= 5.3

# where lua headers are found for macos builds
# LUAINC_macos:
# /opt/local/include
LUAINC_macos_base ?= /usr/local/include
LUALIB_macos_base ?= /usr/local/lib
LUAINC_macos ?= \
	$(LUAINC_macos_base)/lua/$(LUAV) \
	$(LUAINC_macos_base)/lua$(LUAV) \
	$(LUAINC_macos_base)/lua-$(LUAV) \
	$(LUAINC_macos_base)/lilv-0

LIBINC_macos ?= $(LUALIB_macos_base)
LIBS_macos ?= lilv-0

# FIXME default should this default to fink or to macports?
# What happens when more than one Lua version is installed?
LUAPREFIX_macos ?= /usr/local
CDIR_macos ?= lib/lua/$(LUAV)
LDIR_macos ?= share/lua/$(LUAV)

# LUAINC_linux:
# /usr/include/lua$(LUAV)
# /usr/local/include
# /usr/local/include/lua$(LUAV)
# where lua headers are found for linux builds
LUAINC_linux_base ?= /usr/include
LUALIB_linux_base ?= /usr/lib
LUAINC_linux ?= \
	$(LUAINC_linux_base)/lua/$(LUAV) \
	$(LUAINC_linux_base)/lua$(LUAV) \
	$(LUAINC_linux_base)/lilv-0

LIBINC_linux ?= $(LUALIB_linux_base)
LIBS_linux ?= lilv-0

LUAPREFIX_linux ?= /usr/local
CDIR_linux ?= lib/lua/$(LUAV)
LDIR_linux ?= share/lua/$(LUAV)
LIBS_linux ?= lilv-0

# prefix: /usr/local /usr /opt/local /sw
# the top of the default install tree
prefix ?= $(LUAPREFIX_$(PLAT))

CDIR ?= $(CDIR_$(PLAT))
LDIR ?= $(LDIR_$(PLAT))
LIBINC ?= $(LIBINC_$(PLAT))
LIBS ?= $(LIBS_$(PLAT))

# DESTDIR: (no default)
# used by package managers to install into a temporary destination
DESTDIR ?=

#------
# Definitions below can be overridden on the make command line, but
# shouldn't have to be.

#------
# Install directories
#

INSTALL_DIR = install -d
INSTALL_DATA = install -m644
INSTALL_EXEC = install
INSTALL_TOP = $(DESTDIR)$(prefix)

INSTALL_TOP_LDIR = $(INSTALL_TOP)/$(LDIR)
INSTALL_TOP_CDIR = $(INSTALL_TOP)/$(CDIR)

INSTALL_LILV_LDIR = $(INSTALL_TOP_LDIR)/lilv
INSTALL_LILV_CDIR = $(INSTALL_TOP_CDIR)/lilv

print:
	@echo PLAT = $(PLAT)
	@echo LUAV = $(LUAV)
	@echo prefix = $(prefix)
	@echo LUAINC_$(PLAT) = $(LUAINC_$(PLAT))
	@echo LUALIB_$(PLAT) = $(LUALIB_$(PLAT))
	@echo INSTALL_TOP_CDIR = $(INSTALL_TOP_CDIR)
	@echo INSTALL_TOP_LDIR = $(INSTALL_TOP_LDIR)
	@echo CFLAGS = $(CFLAGS)
	@echo LDFLAGS = $(LDFLAGS)

#------
# Supported platforms
#
PLATS = macos linux

#------
# Compiler and linker settings
# for Mac OS X
SO_macos           = so
O_macos            = o
CC_macos           = clang
DEF_macos          =
CFLAGS_macos       = $(LUAINC:%=-I%) $(DEF) -Wall -O2 -fno-common
LDFLAGS_macos      = $(LIBINC:%=-L%) $(LIBS:%=-l%) -bundle -undefined dynamic_lookup -o
LD_macos           = clang
LILV_macos         = lilv.o

#------
# Compiler and linker settings
# for Linux
SO_linux           = so
O_linux            = o
CC_linux           = gcc
DEF_linux          =
CFLAGS_linux       = $(LUAINC:%=-I%) $(DEF) -Wall -Wshadow -Wextra \
	                 -Wimplicit -O2 -ggdb3 -fpic
LDFLAGS_linux      = $(LIBINC:%=-L%) $(LIBS:%=-l%) -O -shared -fpic -o
LD_linux           = gcc
LILV_linux         = lilv.o

#------
# Output file names
#
SO                 = $(SO_$(PLAT))
O                  = $(O_$(PLAT))
LILV_V             = v0
LILV_SO            = lilv.$(SO)
LILV               = $(LILV_$(PLAT))

#------
# Settings selected for platform
#
CC                 = $(CC_$(PLAT))
DEF                = $(DEF_$(PLAT))
CFLAGS             = $(MYCFLAGS) $(CFLAGS_$(PLAT))
LDFLAGS            = $(MYLDFLAGS) $(LDFLAGS_$(PLAT))
LD                 = $(LD_$(PLAT))
LUAINC             = $(LUAINC_$(PLAT))
LUALIB             = $(LUALIB_$(PLAT))

#------
# Modules belonging to LILV-core
#
LILV_OBJS = \
	lua_lilv.$(O) \
	world.$(O) \
	plugin.$(O) \
	plugin_class.$(O)

#------
# Files to install
#
TO_LILV_LDIR =

TO_TOP_LDIR =

#------
# Targets
#
default: $(PLAT)

macos:
	$(MAKE) all PLAT=macos

linux:
	$(MAKE) all PLAT=linux

none:
	@echo "Please run"
	@echo "   make PLATFORM"
	@echo "where PLATFORM is one of these:"
	@echo "   $(PLATS)"

all: $(LILV_SO)

$(LILV_SO): $(LILV_OBJS)
	$(LD) $(LILV_OBJS) $(LDFLAGS)$@

install:
	$(INSTALL_DIR) $(INSTALL_TOP_LDIR)
	$(INSTALL_DATA) $(TO_TOP_LDIR) $(INSTALL_TOP_LDIR)
	$(INSTALL_DIR) $(INSTALL_LILV_LDIR)
	$(INSTALL_DATA) $(TO_LILV_LDIR) $(INSTALL_LILV_LDIR)
	$(INSTALL_DIR) $(INSTALL_LILV_CDIR)
	$(INSTALL_EXEC) $(LILV_SO) $(INSTALL_LILV_CDIR)/core.$(SO)

local:
	$(MAKE) install INSTALL_TOP_CDIR=.. INSTALL_TOP_LDIR=..

clean:
	rm -f $(LILV_SO) $(LILV_OBJS)

.PHONY: all $(PLATS) default clean echo none
