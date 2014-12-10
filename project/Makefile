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

# Extract latest tag (annotated or not) name from git and use it for soname
# versioning. Assumes tags are formed as X.Y.Z, where X, Y, Z are digits, which
# in turn requires that the tag points to HEAD.
VERSION = $(shell git describe --always --tags)
MAJOR_VERSION = $(shell git describe --always --tags | \
					sed 's/^\([0-9]\).*/\1/')

LIBS_NAME_BASE = libsyro_volcasample
SONAME = $(LIBS_NAME_BASE).so.$(VERSION)

SRCS = $(SYRODIR)/korg_syro_comp.c \
	$(SYRODIR)/korg_syro_func.c \
	$(SYRODIR)/korg_syro_volcasample.c \
	$(EXAMPLEDIR)/korg_syro_volcasample_example.c
OBJS = $(SRCS:.c=.o)

$(TARGET) : $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(TARGET)

%.o : %c
	$(CC) -c $(CFLAGS) $< -o $@

libs: $(SYRODIR)/$(LIBS_NAME_BASE).a $(SYRODIR)/$(SONAME)

# static library
$(SYRODIR)/$(LIBS_NAME_BASE).a : $(OBJS)
	$(AR) -cvq $@ $(OBJS)

# shared library
$(SYRODIR)/$(SONAME): $(OBJS)
	$(CC) -shared -Wl,-soname,$(SONAME) -o $@ $(OBJS)
	ln -s $(SYRODIR)/$(SONAME) $(SYRODIR)/$(LIBS_NAME_BASE).so.$(MAJOR_VERSION)
	ln -s $(SYRODIR)/$(SONAME) $(SYRODIR)/$(LIBS_NAME_BASE).so

# clean
clean:
	$(RM) ./$(TARGET)
	$(RM) ./$(TARGET).exe
	$(RM) $(SYRODIR)/*.o $(EXAMPLEDIR)/*.o
	$(RM) $(SYRODIR)/*.a $(SYRODIR)/*.so*

# Listing of phony targets.
.PHONY : clean
