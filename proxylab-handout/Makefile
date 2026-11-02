# Makefile for Proxy Lab 
# 中文：Proxy Lab 的 Makefile
#
# You may modify this file any way you like (except for the handin
# 中文：可以按需修改本文件，但 handin
# rule). You instructor will type "make" on your specific Makefile to
# 中文：规则除外；教师将使用你提供的 Makefile 执行 make
# build your proxy from sources.
# 中文：从源码构建代理程序

CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: proxy

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c

proxy.o: proxy.c csapp.h
	$(CC) $(CFLAGS) -c proxy.c

proxy: proxy.o csapp.o
	$(CC) $(CFLAGS) proxy.o csapp.o -o proxy $(LDFLAGS)

# Creates a tarball in ../proxylab-handin.tar that you can then
# 中文：在上一层生成可提交的 tar 归档，原文示例名为 proxylab-handin.tar
# hand in. DO NOT MODIFY THIS!
# 中文：可将其提交；不要修改此规则
handin:
	(make clean; cd ..; tar cvf $(USER)-proxylab-handin.tar proxylab-handout --exclude tiny --exclude nop-server.py --exclude proxy --exclude driver.sh --exclude port-for-user.pl --exclude free-port.sh --exclude ".*")

clean:
	rm -f *~ *.o proxy core *.tar *.zip *.gzip *.bzip *.gz

