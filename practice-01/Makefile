GCCDIR = /home/codeman/setup/gcc/gcc520/gcc520a/install/bin

CXX = $(GCCDIR)/g++
# Flags for the C++ compiler: C++11 and all the warnings
CXXFLAGS = -std=c++11 -Wall -fno-rtti
# Workaround for an issue of -std=c++11 and the current GCC headers
CXXFLAGS += -Wno-literal-suffix

# Determine the plugin-dir and add it to the flags
PLUGINDIR=$(shell $(CXX) -print-file-name=plugin)
CXXFLAGS += -I$(PLUGINDIR)/include

LDFLAGS = -std=c++11

# top level goal: build our plugin as a shared library
all: warn_unused.so

warn_unused.so: warn_unused.o
	$(CXX) $(LDFLAGS) -shared -o $@ $<

warn_unused.o : warn_unused.cc
	$(CXX) $(CXXFLAGS) -fPIC -c -o $@ $<

clean:
	rm -f *.o *.so

check: warn_unused.so test.cc
	$(CXX) -c -fplugin=./warn_unused.so -c test.cc
 
.PHONY: all clean check
