applib: ArrayDictionary.o ArrayGraph.o

ArrayDictionary.o: ArrayDictionary.cpp
	g++ -std=c++11 -Wall -g -c -o $@ $<

ArrayGraph.o: ArrayGraph.cpp
	g++ -std=c++11 -Wall -g -c -o $@ $<

clean:  
	@-rm -f *.o