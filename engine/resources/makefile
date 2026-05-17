
GLSLC:=glslc

COMPUTE_SOURCE := $(wildcard shaders/*.comp)
COMPUTE_BIN := $(addsuffix .spv, $(COMPUTE_SOURCE))

VERTEX_SOURCE := $(wildcard shaders/*.vert)
VERTEX_BIN := $(addsuffix .spv, $(VERTEX_SOURCE))

FRAGMENT_SOURCE := $(wildcard shaders/*.frag)
FRAGMENT_BIN := $(addsuffix .spv, $(FRAGMENT_SOURCE))

all: $(COMPUTE_BIN) $(FRAGMENT_BIN) $(VERTEX_BIN)

%.comp.spv: %.comp
	$(GLSLC) $< -o $@

%.vert.spv: %.vert
	$(GLSLC) $< -o $@

%.frag.spv: %.frag
	$(GLSLC) $< -o $@

clean:
	rm $(COMPUTE_BIN)
	rm $(VERTEX_BIN)
	rm $(FRAGMENT_BIN)
