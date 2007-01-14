
bins = map_unittest 

#################################################

FLAGS = -O3 -Wall -fomit-frame-pointer -DNDEBUG -fno-exceptions
INCLUDES = -I..
LIBS = -lstdc++

CXXFLAGS= $(FLAGS) $(INCLUDES) 
CXX=g++

sources = $(bins:=.cpp)

all: $(bins)

$(bins): 
	$(CXX) $(FLAGS) $(INCLUDES) $(LIBS)  $< -o $@ 

clean: 
	rm -rf *~ *.d *.o $(bins)

%.d: %.cpp
	set -e; $(CXX) $(INCLUDES) -MM $(CPPFLAGS) $< \
		| sed 's/\($*\)\.o[ :]*/\1 $@ : /g' > $@; \
	        [ -s $@ ] || rm -f $@

include $(sources:.cpp=.d)
