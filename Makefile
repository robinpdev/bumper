
main:
	g++ -o bumperapp src/main.cpp \
		libraries/imgui/imgui.cpp \
		libraries/imgui/imgui_draw.cpp \
		libraries/imgui/imgui_widgets.cpp \
		libraries/imgui/imgui_tables.cpp \
		libraries/imgui/imgui_demo.cpp \
		libraries/imgui/backends/imgui_impl_opengl2.cpp \
		-lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 \
		-I $(shell pwd) -I $(shell pwd)/libraries/imgui