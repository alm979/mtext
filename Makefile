CC=gcc
CF=-Wall -Wextra -pedantic

all:mtext

mtext: mtext.c
	$(CC) $(CF) -o mtext mtext.c

clear:
	rm mtext
