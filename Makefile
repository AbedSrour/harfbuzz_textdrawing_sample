CC		:= gcc
C_FLAGS := -g -O0 -Wall -Wextra `pkg-config --cflags freetype2`

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:= `pkg-config --libs freetype2` -L/usr/lib/x86_64-linux-gnu -lsndfile -lm -lharfbuzz

ifeq ($(OS),Windows_NT)
EXECUTABLE	:= main.exe
else
EXECUTABLE	:= main
endif

all: $(BIN)/$(EXECUTABLE)

clean:
	-$(RM) $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*
	$(CC) $(C_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)