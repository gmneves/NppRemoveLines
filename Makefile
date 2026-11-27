PLUGIN_NAME = NppRemoveLines
SRC = PluginDefinition.cpp NppPluginDemo.cpp
RC = NppPluginDemo.rc
RES = $(RC:.rc=.o)
OUT = $(PLUGIN_NAME).dll

CXX = g++
WINDRES = windres
CXXFLAGS = -Wall -O2 -shared -std=c++17 -DUNICODE -D_UNICODE -municode -I.
LDFLAGS = -static -static-libgcc -static-libstdc++ -Wl,--add-stdcall-alias -lshlwapi

all: $(OUT)

$(RES): $(RC)
	$(WINDRES) $< -o $@

$(OUT): $(SRC) $(RES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	del /f /q *.dll *.def *.o 2>nul || exit 0
