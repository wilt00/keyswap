keyswap: keyswap.c
	gcc -Wall -Wextra -O3 -o keyswap keyswap.c

install: keyswap
	install -m 755 -g root -o root -v ./keyswap /usr/bin/