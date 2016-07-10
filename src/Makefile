CC=g++
override CFLAGS+=-c -Wall --std=c++11 -lglpk
override LDFLAGS+=-lglpk

SOURCES=graph.cpp main.cpp svg_output.cpp

OBJECTS=$(SOURCES:.cpp=.o)
	EXECUTABLE=magic

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
	$(RM) *.svg

