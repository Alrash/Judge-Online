CC=g++
EXE=compiler
SOURCES:=$(wildcard *.cpp ../public/*.cpp)
HEADERS:=$(wildcard *.h ../public/*.h)

executor:$(SOURCES) $(HEADERS)
	$(CC) $(SOURCES) -std=c++11 -o $(EXE) `pkg-config glib-2.0 --cflags --libs`
