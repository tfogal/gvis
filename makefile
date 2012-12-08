STD=-std=c++0x
INC=
WARN=-Wall -Wextra -Werror
CFLAGS=-std=c99 $(WARN) -g -fPIC $(INC)
CXXFLAGS=$(STD) $(WARN) -g -fPIC $(INC)
LIBS=-lpng

include .src
OBJ:=$(SRC:.cpp=.o)
OBJ:=$(OBJ:.c=.o)

all: $(OBJ) test

test: bstream.o cast.o crop.o driver.o filter.o png.o
	$(CXX) $^ -o $@ $(LIBS)

clean:
	rm -f $(OBJ) test
