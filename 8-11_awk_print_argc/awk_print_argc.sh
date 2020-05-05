#!/usr/bin/awk -f

BEGIN {
	for (i = 0; i < ARGC; i++)
		printf "argc[%d] = %s\r\n", i, ARGV[i]
	exit
}
