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
