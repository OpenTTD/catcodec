# $Id$

# This file is part of catcodec.
# catcodec is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
# catcodec is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with catcodec. If not, see <http://www.gnu.org/licenses/>.

ifdef VERBOSE
	Q =
else
	Q = @
endif

AWK = "awk"
ROOT_DIR := $(shell pwd)
BUNDLE_DIR = "$(ROOT_DIR)/bundle"
BUNDLES_DIR = "$(ROOT_DIR)/bundles"
CATCODEC = catcodec$(EXTENSION)
CFLAGS += -Wall -Wcast-qual -Wwrite-strings -Wno-multichar -g -ggdb -Os
OBJS = catcodec.o io.o sample.o
OS = unknown

all: $(CATCODEC)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $^

$(CATCODEC): $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(CATCODEC)

mrproper: clean
	rm -rf $(BUNDLE_DIR) $(BUNDLES_DIR)

include Makefile.bundle
