CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wpedantic -std=c11 -ggdb -D_POSIX_C_SOURCE=200809L

duration: duration.c utils/spinner.c
	$(CC) $(CFLAGS) -o duration duration.c utils/spinner.c -lpthread

clean:
	rm -f duration