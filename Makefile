PREFIX = /home/ajuffer

VPATH = ../include/simploce/surface \
        ../include/simploce/util \
        ../include/simploce \
        ../src \
        ../tests \
        ../apps

TEMPLATES = cvector.hpp id.hpp util.hpp

INCLUDE = conf.hpp \
          surface.hpp triangle.hpp edge.hpp vertex.hpp triangulator.hpp \
          tetrahedron-triangulator.hpp

SRC = surface.cpp triangle.cpp edge.cpp vertex.cpp tetrahedron-triangulator.cpp

TESTS = test-vertex.cpp test-surface.cpp

APPS =

SOBJ = $(SRC:.cpp=.o)
LOBJ = $(SRC:.cpp=.lo)
TOBJ = $(TESTS:.cpp=.o)
TEXE = $(TESTS:.cpp=)
AEXE = $(APPS:.cpp=)

# For g++ >= 5.4
CC = g++
LT = libtool
LNAME = pka
OPT = -ggdb
STD = c++14
#INCLPATH = -I$(CPPUTIL)/include -I$(CONF)/include
INCLPATH =
#OLIBS = -lcpputil
OLIBS = 
CFLAGS = -I../include $(INCLPATH) $(OPT) -Wall -std=$(STD) -pthread
LDFLAGS = -I../include $(INCLPATH) $(OPT) -Wall -std=$(STD) -L. -L$(PREFIX)/lib -pthread
LIBS = -lm -l$(LNAME) -lpthread $(OLIBS) -lboost_program_options -lboost_iostreams -lbz2 -lz
#-lz for zlib

# All object files. Default.
%.o : %.cpp $(INCLUDE)
	$(LT) --mode=compile $(CC) -c $(CFLAGS) $< -o $@

# Special cases
triangle.o : triangle.cpp $(INCLUDE) $(TEMPLATES)
	$(LT) --mode=compile $(CC) -c $(CFLAGS) $< -o $@

# All executables.
% : %.cpp $(INCLUDE) $(TEMPLATES)
	$(LT) --mode=link $(CC) $(LDFLAGS) $(LIBS) $< -o $@ -rpath $(PREFIX)/bin

lib : $(SOBJ)
	$(LT) --mode=link $(CC) $(CFLAGS) -o lib$(LNAME).la $(LOBJ) -rpath $(PREFIX)/lib

tests: $(TEXE)
	echo "Done: tests"

apps: $(AEXE)
	echo "Done: apps"

all: lib tests apps
	echo "Done: all"

install-lib: lib
	$(LT) --mode=install install lib$(LNAME).la $(PREFIX)/lib

install-tests: tests
	$(LT) --mode=install install $(TEXE) $(PREFIX)/bin

install-apps: apps
	$(LT) --mode=install install $(AEXE) $(PREFIX)/bin

install: install-lib install-tests install-apps

doc: $(INCLUDE) $(TEMPLATES)
	cd ..;pwd;/usr/bin/doxygen Doxyfile;

clean-build :
	$(LT) --mode=clean \rm -r -f ../build/.libs/*
	$(LT) --mode=clean \rm -r -f ../build/.libs/*.*
	\rm -r -f ../build/.libs
	$(LT) --mode=clean \rm -r -f ../build/*

clean-tests :
	$(LT) --mode=clean \rm -r -f $(TEXE)

clean-apps:
	$(LT) --mode=clean \rm -r -f $(AEXE)

clean-lib :
	$(LT) --mode=clean \rm -r -f *.lo lib$(LNAME)*

clean-doxy:
	$(LT) --mode=clean \rm -r -f ../html/search/*
	- $(LT) --mode=clean rmdir --verbose ../html/search
	\rm -r -f ../html/search
	$(LT) --mode=clean \rm -r -f ../html/*.html
	$(LT) --mode=clean \rm -r -f ../html/*.js
	$(LT) --mode=clean \rm -r -f ../html/*.*
	- $(LT) --mode=clean \rm -r -f ../html/*
	$(LT) --mode=clean \rm -r -f ../latex/*
	\rm -r -f ../latex/*

clean: clean-tests clean-apps clean-lib clean-build clean-doxy
	echo "Done: clean"
