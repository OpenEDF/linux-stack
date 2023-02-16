
CC := gcc
LIB_NAME := libch36x.so
LIB_FLAG := -shared
OBJ_FILE := ch36x_lib.o

${LIB_NAME}:${OBJ_FILE}
	$(CC) -o $@ $^ ${LIB_FLAG}

%.o:%.c
	$(CC) -c $< -o $@

install:
	mv ${LIB_NAME} /lib/

clean:
	rm -rf *.o ${LIB_NAME}


