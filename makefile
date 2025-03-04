SRC_DIR = src
BIN_DIR = bin
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(BIN_DIR)/%.o, $(SRC))

PROG_NAME = main

CC = g++
FLAGS = -pedantic-errors -std=c++11

$(BIN_DIR)/$(PROG_NAME): $(OBJ)
	$(CC) $(FLAGS) -o $@ $^ -lncurses

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(FLAGS) -c -o $@ $<

clean:
	rm -f $(BIN_DIR)/*.o
	rm -f $(BIN_DIR)/$(PROG_NAME)

.PHONY: clean
