# #############################################################################
# 
# Makefile for SYRO SDK
# Copyright (C) 2014 Korg Inc.
#
# #############################################################################
TARGET = syro_volcasample_example

CC = gcc
AR = ar
RM = rm -f
CFLAGS = -O3 -Wall -W -Wformat=2 -Wcast-qual -Wcast-align -Wwrite-strings \
		 -Wconversion -Wfloat-equal -Wpointer-arith -fPIC
LDFLAGS = 

SYRODIR = ../syro
EXAMPLEDIR = ../example

SRCS = $(SYRODIR)/korg_syro_comp.c \
	   $(SYRODIR)/korg_syro_func.c \
	   $(SYRODIR)/korg_syro_volcasample.c \
	   $(EXAMPLEDIR)/korg_syro_volcasample_exsample.c
OBJS = $(SRCS:.c=.o)

$(TARGET) : $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(TARGET)

%.o : %c
	$(CC) -c $(CFLAGS) $< -o $@

libs: $(SYRODIR)/libsyro.a $(SYRODIR)/libsyro.so.1.0.0

$(SYRODIR)/libsyro.a : $(OBJS)
	$(AR) -cvq $@ $(OBJS)

$(SYRODIR)/libsyro.so.1.0.0: $(OBJS)
	$(CC) -shared -Wl,-soname,$@ -o $@ $(OBJS)
	ln -s $(SYRODIR)/libsyro.so.1.0.0 $(SYRODIR)/libsyro.so.1
	ln -s $(SYRODIR)/libsyro.so.1.0.0 $(SYRODIR)/libsyro.so

# clean
clean:
	$(RM) ./$(TARGET)
	$(RM) ./$(TARGET).exe
	$(RM) $(SYRODIR)/*.o $(EXAMPLEDIR)/*.o
	$(RM) $(SYRODIR)/*.a $(SYRODIR)/*.so*

# Listing of phony targets.
.PHONY : clean
