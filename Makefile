PLUGIN_NAME = NppRemoveLines
SRC = PluginDefinition.cpp NppPluginDemo.cpp
OUT = $(PLUGIN_NAME).dll

CXX = g++
CXXFLAGS = -Wall -O2 -shared -std=c++17 -DUNICODE -D_UNICODE -municode \
           -I. -I"C:/gui/npp_plg/NppRemoveLines"

LDFLAGS = -static -static-libgcc -static-libstdc++ -Wl,--add-stdcall-alias -lshlwapi

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	del *.dll *.def