keyswap: keyswap.c
	gcc -Wall -Wextra -O3 -o keyswap keyswap.c

install: keyswap
	mv ./keyswap /usr/bin/