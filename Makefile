CC = gcc
CFLAGS = -g -Wall
INCLUDES = -I./include
LFLAGS = 
LIBS = -lcurl -ljson
SRCS = src/main.c src/common.c src/cli.c src/api.c src/install.c
OBJS = $(SRCS:.c=.o)
MAIN = eg

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS) 

.c.o: 
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean: 
	rm -f $(MAIN)
	rm -f *.exe
	rm -f src/*.o
	
depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
