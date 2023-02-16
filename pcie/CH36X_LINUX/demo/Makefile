# Makefile for WCH CH36x chips 

CC := gcc
OBJ := DEMO
OBJ_INT := DEMO_INT
OBJ_CMD := DEMO_CMD
OBJ_GUI := DEMO_GUI_GTK
OBJ_C := ch36x_demo.o ch36x_lib.o
OBJ_C_INT := ch36x_demo_INT.o ch36x_lib.o
OBJ_C_CMD := ch36x_demo_CMD.o ch36x_lib.o
OBJ_C_GUI := ch36x_demo_GUI.o ch36x_lib.o
CFLAGS := -Wall -g 
CFLAGS_GUI := -Wall -g `pkg-config --cflags --libs gtk+-2.0`

${OBJ}:${OBJ_C}
	$(CC) -o $@ $^ ${CFLAGS}
	$(CC) -c ch36x_lib.c ${CFLAGS}
	$(CC) -c ch36x_demo_INT.c ${CFLAGS}
	$(CC) -o ${OBJ_INT} ${OBJ_C_INT} ${CFLAGS}
	$(CC) -c ch36x_demo_CMD.c ${CFLAGS}
	$(CC) -o ${OBJ_CMD} ${OBJ_C_CMD} ${CFLAGS}
	$(CC) -c ch36x_demo_GUI.c ${CFLAGS_GUI}
	$(CC) -o ${OBJ_GUI} ${OBJ_C_GUI} ${CFLAGS_GUI}

#DEMO:${OBJ_C}
#$(CC) -o $@ $^ ${CFLAGS}

#DEMO_GUI_GTK:
#$(CC) -c ch36x_lib.c ${CFLAGS}
#$(CC) -c ch36x_demo_GUI.c ${CFLAGS_GUI}
#$(CC) -o $@ ${OBJ_C_GUI} ${CFLAGS_GUI}

%.o:%.c
	$(CC) -c $< ${CFLAGS}

clean:
	rm -rf *.o ${OBJ} ${OBJ_GUI} ${OBJ_CMD} ${OBJ_INT}
