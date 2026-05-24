
GLSLC:=glslc
SLANGC:=slangc

COMPUTE_SOURCE := $(wildcard shaders/*.comp)
COMPUTE_BIN := $(addsuffix .spv, $(COMPUTE_SOURCE))

VERTEX_SOURCE := $(wildcard shaders/*.vert)
VERTEX_BIN := $(addsuffix .spv, $(VERTEX_SOURCE))

FRAGMENT_SOURCE := $(wildcard shaders/*.frag)
FRAGMENT_BIN := $(addsuffix .spv, $(FRAGMENT_SOURCE))

SLANG_SOURCE := $(wildcard shaders/*.slang)
SLANG_BIN := $(addsuffix .spv, $(SLANG_SOURCE))

all: $(COMPUTE_BIN) $(FRAGMENT_BIN) $(VERTEX_BIN) $(SLANG_BIN)

%.comp.spv: %.comp
	$(SLANGC) $< -target spirv -entry main -o $@

%.vert.spv: %.vert
	$(SLANGC) $< -target spirv -entry main -o $@

%.frag.spv: %.frag
	$(SLANGC) $< -target spirv -entry main -o $@

%.slang.spv: %.slang
	$(SLANGC) $< -target spirv -o $@

clean:
	rm $(COMPUTE_BIN)
	rm $(VERTEX_BIN)
	rm $(FRAGMENT_BIN)
	rm $(SLANG_BIN)
