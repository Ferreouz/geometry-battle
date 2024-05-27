CXX := g++
OUTPUT := runme

SFML_DIR := .

CXX_FLAGS := -O3 -std=c++17
INCLUDES := -I./src -I$(SFML_DIR)/include
LDFLAGS := -O3 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -L$(SFML_DIR)/lib

SRC_FILES := $(wildcard src/*.cpp)
OBJ_FILES :=  $(SRC_FILES:.cpp=.o)

all:$(OUTPUT)
	./bin/runme

$(OUTPUT):$(OBJ_FILES) Makefile
	$(CXX) $(OBJ_FILES) $(LDFLAGS) -o ./bin/$@

.cpp.o:
	$(CXX) -c $(CXX_FLAGS) $(INCLUDES) $< -o $@

clean:
	rm -f ./bin/runme
	rm -f src/*.o
