target := labyrinth
objs := main.o array_stuff.o iawls.o input_processing.o bitvectors.o hash.o queue.o bfs.o char_handling.o

CC := gcc
CFLAGS := -Wall -Wextra -g -Wno-implicit-fallthrough -std=c17 -O2 

all: $(target)

deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

labyrinth: $(objs)
	$(CC) $(CCFLAGS) -o $@ $^ -lm

%.o: %.c %.h
	$(CC) $(CCFLAGS) -c $< -lm $(DEPFLAGS)

clean:
	rm -f $(target) $(objs) $(deps)