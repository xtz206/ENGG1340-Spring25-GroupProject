SRC_DIR = src
BIN_DIR = bin
DIST_DIR = dist
ASSETS_DIR = assets

CXX = gcc
CXXFLAGS = -std=c++11 -pedantic-errors
LDFLAGS = -lstdc++ -lncursesw
PROG = main

$(BIN_DIR)/$(PROG): $(BIN_DIR)/main.o $(BIN_DIR)/render.o $(BIN_DIR)/control.o
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/render.h
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/render.o: $(SRC_DIR)/render.cpp $(SRC_DIR)/render.h
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/control.o: $(SRC_DIR)/control.cpp $(SRC_DIR)/control.h
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(BIN_DIR)/map.o: $(SRC_DIR)/map.cpp $(SRC_DIR)/map.h
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -c $< -o $@


dist: $(BIN_DIR)/$(PROG) assets/*
	@mkdir -p $(DIST_DIR)
	cp $(BIN_DIR)/$(PROG) $(DIST_DIR)/$(PROG)
	cp $(ASSETS_DIR)/* $(DIST_DIR)/

run: dist
	cd $(DIST_DIR) && ./$(PROG)

clean:
	rm -f $(BIN_DIR)/*.o
	rm -f $(PROG)
	rm -f $(DIST_DIR)/*

.PHONY: dist, run, clean