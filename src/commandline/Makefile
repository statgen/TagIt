applib: CommandLine.o TagItCommandLine.o

CommandLine.o: CommandLine.cpp
	g++ -std=c++11 -Wall -g -c -o $@ $<

TagItCommandLine.o: TagItCommandLine.cpp
	g++ -std=c++11 -Wall -g -c -o $@ $<

clean:  
	@-rm -f *.o