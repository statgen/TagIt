applib:	Writer.o TextWriter.o GzipWriter.o WriterFactory.o

Writer.o: Writer.cpp
	g++ -std=c++11 -Wall -c -o $@ $<
			
TextWriter.o: TextWriter.cpp
	g++ -std=c++11 -Wall -c -o $@ $<
			
GzipWriter.o: GzipWriter.cpp
	g++ -std=c++11 -Wall -isystem/usr/include -c -o $@ $<
			
WriterFactory.o: WriterFactory.cpp
	g++ -std=c++11 -Wall -c -o $@ $<

clean:  
	@-rm -f *.o
