CC=g++
CFLAGS=`pkg-config --cflags opencv`
LDFLAGS=`pkg-config --libs opencv`

PROG=facerec
OBJS=$(PROG).o

.PHONY: all clean
$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $(CFLAGS) $<

all:
	$(PROG)

clean:
	rm -f $(OBJS) $(PROG)

# CC = g++

# CPPFLAGS = -g -Wall -Wno-unused-function `pkg-config --cflags opencv`

# LDFLAGS = -Wl,--no-as-needed `pkg-config --libs opencv`

# OBJ = $(SRC:.cpp=.o)

# EXPENDABLES = facerec

# all: $(EXPENDABLES)

# facerec.o: facerec.cpp
# 	$(CC) $(CPPFLAGS) -c facerec.cpp

# facerec: facerec.o
# 	$(CC) $(LDFLAGS) -o facerec facerec.o

# clean:
# 	rm -f *.o $(EXPENDABLES)
