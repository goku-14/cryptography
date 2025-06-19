.PHONY:clean

CC = gcc
CFLAGS = -Wall -g -O2
INCLUDE = -I.
DLL_TARGET = libibe.so
DLL_OBJS = hash.o ibe.o keygen.o mac.o sign.o utils.o aes.o
DLL_SRCS = hash.c ibe.c keygen.c mac.c sign.c utils.c aes.c
DLL_DEPS = -L. -lgmp -L/usr/local/lib -lpbc
TEST_TARGET = test
TEST_OBJS = test.o
TEST_SRCS = test.c
SPEED_TARGET = speed
SPEED_OBJS = speed.o
SPEED_SRCS = speed.c
LDFLAGS = -Wl,-rpath=.
TARGET_DEPS = -L. -libe -L. -lgmp -L/usr/local/lib -lpbc

dll : $(DLL_OBJS)
	$(CC) -fPIC -shared $(CFLAGS) $(DLL_OBJS) -o $(DLL_TARGET) $(DLL_DEPS) $(LDFLAGS) 

$(DLL_OBJS) : $(DLL_SRCS)
	$(CC) -fPIC $(CFLAGS) $(INCLUDE) -c $^

test : $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_OBJS) $(TARGET_DEPS) -o $(TEST_TARGET) $(LDFLAGS) 

$(TEST_OBJS) : $(TEST_SRCS)
	$(CC) $(CFLAGS) $(INCLUDE) -c $^

speed : $(SPEED_OBJS)
	$(CC) $(CFLAGS) $(SPEED_OBJS) $(TARGET_DEPS) -o $(SPEED_TARGET) $(LDFLAGS) 

$(SPEED_OBJS) : $(SPEED_SRCS)
	$(CC) $(CFLAGS) $(INCLUDE) -c $^

all : dll test speed

clean:
	rm -f *.o
	rm -f $(DLL_TARGET)
	rm -f $(TEST_TARGET)
