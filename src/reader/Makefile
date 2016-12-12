applib:	Reader.o TextReader.o GzipReader.o ReaderFactory.o

Reader.o: Reader.cpp
	g++ -std=c++11 -Wall -g -c -o $@ $<
			
TextReader.o: TextReader.cpp
	g++ -std=c++11 -Wall -g -c -o $@ $<
			
GzipReader.o: GzipReader.cpp
	g++ -std=c++11 -Wall -isystem/usr/include -g -c -o $@ $<
			
ReaderFactory.o: ReaderFactory.cpp
	g++ -std=c++11 -Wall -g -c -o $@ $<

clean:  
	@-rm -f *.o
