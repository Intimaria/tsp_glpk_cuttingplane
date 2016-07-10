CC=g++
override CFLAGS+=-c -Wall --std=c++11
override CFLAGSL+=
override LDFLAGS+=

SOURCES=graph.cpp main.cpp svg_output.cpp

OBJECTS=$(SOURCES:.cpp=.o)
	EXECUTABLE=magic

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(CFLAGSL)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ $(CFLAGSL)

.PHONY: clean

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
	$(RM) *.svg

