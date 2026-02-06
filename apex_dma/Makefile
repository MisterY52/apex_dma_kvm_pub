CXX=g++
CXXFLAGS=-I./memflow_lib/memflow-ffi/ -L./memflow_lib/target/release -Wno-multichar
LIBS=-lm -ldl -lpthread -l:libmemflow_ffi.a

OUTDIR=./build
OBJDIR=$(OUTDIR)/obj

$(shell mkdir -p $(OBJDIR))

%.o: %.cpp
	$(CXX) -c -o $(OBJDIR)/$@ $< $(CXXFLAGS)

apex_dma: apex_dma.o Game.o Math.o memory.o offsets_dynamic.o
	$(CXX) -o $(OUTDIR)/$@ $(OBJDIR)/apex_dma.o $(OBJDIR)/Game.o $(OBJDIR)/Math.o $(OBJDIR)/memory.o $(OBJDIR)/offsets_dynamic.o $(CXXFLAGS) $(LIBS)

.PHONY: all
all: apex_dma

.DEFAULT_GOAL := all

clean:
	rm -rf $(OUTDIR)
