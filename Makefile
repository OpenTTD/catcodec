# $Id$

# =========================================================
# Makefile for the catcodec program
#
#       Don't put any local configuration in here
#       Change Makefile.local instead, it'll be
#       preserved when updating the sources
# =========================================================
#
# catcodec is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
# catcodec is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with catcodec. If not, see <http://www.gnu.org/licenses/>.

ifdef VERBOSE
	Q =
else
	Q = @
endif


ROOT_DIR := $(shell pwd)
PACKAGE_NAME = catcodec

-include Makefile.local

AWK          ?= "awk"
BUNDLE_DIR   ?= "$(ROOT_DIR)/bundle"
BUNDLES_DIR  ?= "$(ROOT_DIR)/bundles"
CATCODEC     ?= catcodec$(EXTENSION)
CXXFLAGS     ?= -Wall -Wcast-qual -Wwrite-strings
OS           ?= unknown


OBJS = catcodec.o io.o sample.o rev.o

# Regardless of the warning settings, we really do not want these errors.
CXXFLAGS += -Wno-multichar
ifdef DEBUG
	CXXFLAGS += -g -ggdb
endif

all: $(CATCODEC)

objs/%.o: src/%.cpp
	$(Q)mkdir -p objs
	@echo '[CPP] $@'
	$(Q)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(CATCODEC): $(OBJS:%=objs/%)
	@echo '[LINK] $@'
	$(Q)$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $^

VERSION := $(shell ./findversion.sh | cut -f 1 -d'	')
RES := $(shell if [ "`cat version.cache 2>/dev/null`" != "$(VERSION)" ]; then echo "$(VERSION)" > version.cache; fi )

src/rev.cpp: version.cache src/rev.cpp.in
	$(Q)cat src/rev.cpp.in | sed "s@\!\!VERSION\!\!@$(VERSION)@g" > src/rev.cpp

clean:
	@echo '[CLEAN]'
	$(Q)rm -f $(CATCODEC) src/rev.cpp version.cache
	$(Q)rm -rf objs

mrproper: clean
	$(Q)rm -rf $(BUNDLE_DIR) $(BUNDLES_DIR)

include Makefile.bundle
