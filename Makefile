# Add the names of your .c files from your assignment here.
SRCS = simple_shell.c utils.c

# This line is the name of the program you are building
BIN = simple_shell

# Compiler flags to pass to the system's C compiler while building
# the source files
CFLAGS = -g -Wall -Wextra -pedantic -std=gnu11

# Flags to pass to the linker while linking your program
# You shouldn't need to change this at all.
LDFLAGS = -g

###################################################################
# You shouldn't need to touch anything below this line...         #
###################################################################
OBJS = $(SRCS:.c=.o)

all: $(BIN)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

clean:
	-rm -f $(OBJS)
	-rm -f $(BIN)