src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)
bin = bin/

LDFLAGS = -lsfml-system -lsfml-window -lsfml-graphics
CXXFLAGS = -O

palletone: $(obj)
	mkdir -p $(bin)
	$(CXX) -o $(bin)$@ $^ $(CXXFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) $(bin)palletone

