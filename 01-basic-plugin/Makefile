.DEFAULT_GOAL := test

GCCDIR = /home/codeman/setup/gcc/gcc520/gcc520a/install/bin
PLUGINNAME=hello-plugin

CXX = $(GCCDIR)/g++
# Flags for the C++ compiler: enable C++11 and all the warnings, -fno-rtti is required for GCC plugins
CXXFLAGS = -std=c++11 -Wall -fno-rtti 
# Workaround for an issue of -std=c++11 and the current GCC headers
CXXFLAGS += -Wno-literal-suffix

# Determine the plugin-dir and add it to the flags
PLUGINDIR = $(shell $(CXX) -print-file-name=plugin)
CXXFLAGS += -I$(PLUGINDIR)/include

TESTFILE = test.c

# top level goal: build our plugin as a shared library
all: $(PLUGINNAME).so

$(PLUGINNAME).so: $(PLUGINNAME).o
	$(CXX) $(LDFLAGS) -shared -o $@ $<

$(PLUGINNAME).o : $(PLUGINNAME).cc
	$(CXX) $(CXXFLAGS) -fPIC -c -o $@ $<

clean:
	rm -f $(PLUGINNAME).o $(PLUGINNAME).so

test: $(PLUGINNAME).so
	$(CXX) -fplugin=./$(PLUGINNAME).so -c -x c++ $(TESTFILE) -o /dev/null

.PHONY: all clean check
