CXXFLAGS=-std=c++11

all: control

conf: confMemory.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lrt

control: mmu.o
	$(CXX) $(CXXFLAGS) -o $@ $^ -lrt

confMemory.o: confMemory.cpp

mmu.o: mmu.cpp

clean:
	rm -f control conf
	rm -f *.o *.*~ *~ 
