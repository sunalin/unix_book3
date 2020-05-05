#!/usr/bin/awk -f

# 请注意UNIX的换行符是\n,使用git拉取还原文件时需注意
BEGIN {
	for (i = 0; i < ARGC; i++)
		printf "argc[%d] = %s\r\n", i, ARGV[i]
	exit
}
