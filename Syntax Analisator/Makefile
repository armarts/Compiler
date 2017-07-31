CC = g++ -std=c++11
CFLAGS = -c
TARGET = result
SOURCES = syntaxAnalisator.cpp tree.cpp main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: install clean

obj: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES)

install: obj
	$(CC) -o $(TARGET) $(OBJECTS)

clean:
	rm -rf $(OBJECTS) $(TARGET)
