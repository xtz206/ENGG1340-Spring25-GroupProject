SRC_DIR = src
BIN_DIR = bin

CXX = gcc
CXXFLAGS = -std=c++11 -pedantic-errors -Wall
LDFLAGS = -lstdc++ -lncurses
PROG = main

$(BIN_DIR)/$(PROG): $(BIN_DIR)/main.o $(BIN_DIR)/render.o
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/render.h
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/render.o: $(SRC_DIR)/render.cpp $(SRC_DIR)/render.h
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(BIN_DIR)/$(PROG)
	./$(BIN_DIR)/$(PROG)

clean:
	rm -f $(BIN_DIR)/*.o
	rm -f $(PROG)

.PHONY: clean, run