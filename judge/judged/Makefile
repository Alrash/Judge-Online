CC=g++
EXE=judged
SOURCES:=$(wildcard *.cpp ../public/*.cpp)
HEADERS:=$(wildcard *.h ../public/*.h)

executor:$(SOURCES) $(HEADERS)
	$(CC) $(SOURCES) -o $(EXE) -std=c++11 -L /usr/lib/mysql -l mysqlclient `pkg-config glib-2.0 --cflags --libs`
