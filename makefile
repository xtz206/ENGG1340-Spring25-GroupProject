SRC_DIR = src
BIN_DIR = bin
DIST_DIR = dist
ASSETS_DIR = assets

CXX = g++
CXXFLAGS = -std=c++11 -pedantic-errors
LDFLAGS = -lstdc++ -lncursesw
PROG = main

$(BIN_DIR)/$(PROG): $(BIN_DIR)/main.o $(BIN_DIR)/game.o $(BIN_DIR)/render.o $(BIN_DIR)/control.o $(BIN_DIR)/loader.o
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/game.h $(SRC_DIR)/render.h $(SRC_DIR)/control.h $(SRC_DIR)/loader.h
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/game.o: $(SRC_DIR)/game.cpp $(SRC_DIR)/game.h
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/render.o: $(SRC_DIR)/render.cpp $(SRC_DIR)/render.h $(SRC_DIR)/game.h
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/control.o: $(SRC_DIR)/control.cpp $(SRC_DIR)/control.h $(SRC_DIR)/game.h
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/loader.o: $(SRC_DIR)/loader.cpp $(SRC_DIR)/loader.h
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -c $< -o $@

assets: $(ASSETS_DIR)/*
	@mkdir -p $(DIST_DIR)
	cp $(ASSETS_DIR)/* $(BIN_DIR)/

release: CXXFLAGS += -O2
release: clean $(BIN_DIR)/$(PROG) assets
	@mkdir -p $(DIST_DIR)
	cp $(BIN_DIR)/$(PROG) $(DIST_DIR)/$(PROG)
	cp $(ASSETS_DIR)/* $(DIST_DIR)/
	@echo "release build complete"

debug: CXXFLAGS += -g
debug: clean $(BIN_DIR)/$(PROG) assets
	@mkdir -p $(DIST_DIR)
	cp $(BIN_DIR)/$(PROG) $(DIST_DIR)/$(PROG)
	cp $(ASSETS_DIR)/* $(DIST_DIR)/
	@echo "debug build complete"

all: clean $(BIN_DIR)/$(PROG) assets
	@mkdir -p $(DIST_DIR)
	cp $(BIN_DIR)/$(PROG) $(DIST_DIR)/$(PROG)
	cp $(ASSETS_DIR)/* $(DIST_DIR)/
	@echo "all build complete"

clean:
	rm -f $(BIN_DIR)/*.o
	rm -f $(PROG)
	rm -f $(DIST_DIR)/*

.PHONY: assets, release, debug, all, clean
