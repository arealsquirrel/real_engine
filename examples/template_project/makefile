

# stuff that you have to care about
REAL_ENGINE_BUILD_PATH = ../../build
REAL_ENGINE_RESOURCE_PATH = ../../engine/resources/resource_pack.json
REAL_ENGINE_INCLUDE_PATH = ../../engine/include
CXX = clang++
CXX_FLAGS = -O2
CXX_INCLUDE = -I src/game
CXX_LIBS = 
SRC_DIR=src/game
OBJ_DIR=bin/obj


# Compilicated makefile stuff
REAL_ENGINE_LIB_PATH = $(REAL_ENGINE_BUILD_PATH)/engine/libreallib.so
REAL_ENGINE_DEPS_PATH = $(REAL_ENGINE_BUILD_PATH)/_deps

CXX_INCLUDE := $(CXX_INCLUDE) -I $(REAL_ENGINE_INCLUDE_PATH) \
			   -I $(REAL_ENGINE_DEPS_PATH)/entt-src/src \
			   -I $(REAL_ENGINE_DEPS_PATH)/imgui-src \
			   -I $(REAL_ENGINE_DEPS_PATH)/glm-src

CXX_LIBS := $(CXX_LIBS) -L $(REAL_ENGINE_LIB_PATH)

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

GAME_NAME:=mygame

.PHONY: all clean

all:
	mkdir -p logs
	mkdir -p bin
	mkdir -p $(OBJ_DIR)
	make game

game: bin/$(GAME_NAME).so

bin/$(GAME_NAME).so: $(OBJS)
	$(CXX) $(CXX_LIBS) -shared -o bin/$(GAME_NAME).so $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXX_FLAGS) -fPIC $(CXX_INCLUDE) -c $< -o $@

run: bin/$(GAME_NAME).so
	$(REAL_ENGINE_BUILD_PATH)/runtime/real_runtime --game bin/$(GAME_NAME).so

edit: bin/$(GAME_NAME).so
	$(REAL_ENGINE_BUILD_PATH)/editor/real_editor --game bin/$(GAME_NAME).so

clean:
	rm bin/$(GAME_NAME).so
	rm -r bin
	rm -r logs

renderdoc:
	renderdoccmd capture -d . -c logs/renderdoc_capture -w $(REAL_ENGINE_BUILD_PATH)/editor/real_editor --game bin/$(GAME_NAME).so

