CC		:= gcc
C_FLAGS := -g -O0 -Wall -Wextra `pkg-config --cflags harfbuzz freetype2`

BIN		:= .
SRC		:= src
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:= `pkg-config --libs freetype2` -L/usr/lib/x86_64-linux-gnu -lsndfile -lm -lharfbuzz

EXECUTABLE	:= app

all: $(BIN)/$(EXECUTABLE)

clean:
	-$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*
	$(CC) $(C_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)