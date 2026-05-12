CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wpedantic \
		 -std=c11 -ggdb -O2 \
		 -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE 
		 
duration: duration.c utils/spinner.c
	$(CC) $(CFLAGS) -o duration duration.c utils/spinner.c -lavformat -lavutil -lpthread

clean:
	rm -f duration