CC=g++
CFLAGS=`pkg-config --cflags opencv`
LDFLAGS=`pkg-config --libs opencv`

PROG=bag_of_keypoints
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

# EXPENDABLES = bag_of_keypoints

# all: $(EXPENDABLES)

# bag_of_keypoints.o: bag_of_keypoints.cpp
# 	$(CC) $(CPPFLAGS) -c bag_of_keypoints.cpp

# bag_of_keypoints: bag_of_keypoints.o
# 	$(CC) $(LDFLAGS) -o bag_of_keypoints bag_of_keypoints.o

# clean:
# 	rm -f *.o $(EXPENDABLES)

