IDIR =.
CC=g++
CFLAGS=-I$(IDIR) -I. -I./libraries/imgui

ODIR=build/obj
SRCDIR=src
IMGUIDIR=libraries/imgui
LDIR =../lib

LIBS=-lm

_DEPS = src/bump-module.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_SRCOBJ = main.o
SRCOBJ = $(patsubst %,$(ODIR)/%,$(_SRCOBJ))

_IMGUIOBJ = imgui.o imgui_draw.o imgui_tables.o imgui_widgets.o imgui_demo.o imgui_impl_opengl2.o imgui_impl_opengl3.o
IMGUIOBJ = $(patsubst %,$(ODIR)/%,$(_IMGUIOBJ))

$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(IMGUIDIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(IMGUIDIR)/backends/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

# bumper editor
bumped: $(SRCOBJ) $(IMGUIOBJ)
	$(CC) -o $@.app $^ $(CFLAGS) \
		-lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 \
		-rdynamic -g3

# compile the editor from scratch
slowcompileeditor: $(OBJ)
	g++ -o bumped.app src/main.cpp \
		-rdynamic -g3 \
		libraries/imgui/imgui.cpp \
		libraries/imgui/imgui_draw.cpp \
		libraries/imgui/imgui_widgets.cpp \
		libraries/imgui/imgui_tables.cpp \
		libraries/imgui/imgui_demo.cpp \
		libraries/imgui/backends/imgui_impl_opengl2.cpp \
		-lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 \
		-I $(shell pwd) -I $(shell pwd)/libraries/imgui -I $(shell pwd)/libraries/imgui/backends

# compile the test module
module:
	g++ -o build/modules/testmodule.so src/modules/testmodule.cpp -g3\
		-fPIC -shared -rdynamic -std=c++17 \
		-I $(shell pwd)

# compile to wasm
wasmain:
	em++ -std=c++17 -O2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG=1 \
	src/main.cpp  \
	libraries/imgui/imgui.cpp \
	libraries/imgui/imgui_draw.cpp \
	libraries/imgui/imgui_widgets.cpp \
	libraries/imgui/imgui_tables.cpp \
	libraries/imgui/imgui_demo.cpp \
	libraries/imgui/backends/imgui_impl_opengl3.cpp \
	-I $(shell pwd) -I $(shell pwd)/libraries/imgui -I $(shell pwd)/libraries/imgui/backends \
	--preload-file ./build/modules/testmodule.wasm \
	-sMAIN_MODULE \
	-o pge.html

# compile test module to wasm
wasmodule:
	em++ -std=c++17 -O2 -s ALLOW_MEMORY_GROWTH=1 -s \
	src/modules/testmodule.cpp  \
	-sSIDE_MODULE \
	-o build/modules/testmodule.wasm