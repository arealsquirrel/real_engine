
SLANGC:=slangc

SLANG_SOURCE := $(wildcard shaders/*.slang)
SLANG_BIN := $(addsuffix .spv, $(SLANG_SOURCE))

all: $(COMPUTE_BIN) $(FRAGMENT_BIN) $(VERTEX_BIN) $(SLANG_BIN)
	mv $(SLANG_BIN) shaders/bin

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
