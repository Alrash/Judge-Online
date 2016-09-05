CC=g++
EXE=judge
SOURCES=$(wildcard *.cpp ../public/readConfig.cpp ../public/functions.cpp)
HEADERS=../public/readConfig.h ../public/functions.h ../public/config.h

executor:$(SOURCES) $(HEADERS)
	$(CC) -std=c++11 `pkg-config glib-2.0 --cflags --libs` $(SOURCES) -o $(EXE)
