CC=g++
#CFLAGS=-O0 -Wno-deprecated -Wall -c
CFLAGS=-O3 -funroll-loops -Wno-deprecated -Wall -c
#LDFLAGS=-O0 -lm
LDFLAGS=-O2 -lm
SOURCES=Ant.cc Location.cc Threat.cc SearchNode.cc Utility.cc GameState.cc Realm.cc AStar.cc Bot.cc MyBot.cc
OBJECTS=$(SOURCES:.cc=.o)
EXECUTABLE=MyBot

#Uncomment the following to enable debugging
#CFLAGS+=-g -DDEBUG

all: $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cc.o: *.h
	$(CC) $(CFLAGS) $< -o $@

clean: 
	-rm -f ${EXECUTABLE} ${OBJECTS} *.d
	-rm -f debug.txt

.PHONY: all clean

