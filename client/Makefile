SOURCE:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCE))
ELF:=main
CC:=gcc
CFLAGS:=-g -Wall
$(ELF):$(OBJS)
	gcc $^ -o $@ -lcrypt -lcrypto
.PHONY:clean
clean:
	rm -rf $(ELF) $(OBJS) 

	

