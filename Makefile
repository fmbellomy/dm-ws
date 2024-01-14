CXX=/usr/bin/clang
all:
	$(CXX) -shared -fPIC main.cpp -o hyprland-dual-monitor-workspaces.so -g `pkg-config --cflags pixman-1 libdrm hyprland` -std=c++2b -O2
clean:
	rm ./hyprland-dual-monitor-workspaces.so
