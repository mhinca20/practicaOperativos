CXXFLAGS=-std=c++11

control: control.o interpreter.o
	$(CXX) -o $@ $^ -lrt -lpthread $(CXXFLAGS)

control.o: control.cpp control.h interpreter.h

interpreter.o: interpreter.cpp interpreter.h

clean:
	rm -f control interpreter
	rm -f *.o *.*~ *~
