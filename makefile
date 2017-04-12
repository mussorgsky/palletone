src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)
bin = bin

LDFLAGS = -lsfml-system -lsfml-window -lsfml-graphics
CXXFLAGS = -O -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused

palletone: $(obj)
	mkdir -p $(bin)
	$(CXX) -o $(bin)/$@ $^ $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) $(bin)/palletone

