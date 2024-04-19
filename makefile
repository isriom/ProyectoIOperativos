CC = gcc
CFLAGS = -Wall -Wextra -pthread
LDFLAGS = -lrt -pthread

all: Creator Client

Creator: Creator.c
	$(CC) $(CFLAGS) -o Creator Creator.c $(LDFLAGS)

Client: Client.c
	$(CC) $(CFLAGS) -o Client Client.c $(LDFLAGS)

clean:
	rm -f Creator Client
