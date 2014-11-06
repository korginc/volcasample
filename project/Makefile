# #############################################################################
# 
# Makefile for SYRO SDK
# Copyright (C) 2014 Korg Inc.
#
# #############################################################################
TARGET = syro_volcasample_exsample

CC = gcc
RM = rm -f
CFLAGS = -O3 -Wall -W -Wformat=2 -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wfloat-equal -Wpointer-arith
LDFLAGS = 

SYRODIR = ../syro
EXAMPLEDIR = ../example

SRCS = $(SYRODIR)/korg_syro_comp.c $(SYRODIR)/korg_syro_func.c $(SYRODIR)/korg_syro_volcasample.c $(EXAMPLEDIR)/korg_syro_volcasample_exsample.c
OBJS = $(SRCS:.c=.o)

$(TARGET) : $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(TARGET)

%.o : %c
	$(CC) -c $(CFLAGS) $< -o $@

# clean
clean:
	$(RM) ./$(TARGET)
	$(RM) ./$(TARGET).exe
	$(RM) $(SYRODIR)/*.o $(EXAMPLEDIR)/*.o

# Listing of phony targets.
.PHONY : clean 
