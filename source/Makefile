CC=g++
OMPFLAG=-fopenmp
CPP11FLAG=-std=c++11
CFLAGS=$(CPP11FLAG) $(OMPFLAG) -c -Wall
LDFLAGS=$(OMPFLAG)
SOURCES=main.cpp utils.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=words.exe

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)