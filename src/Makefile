LIBS = -lz

APPDIRS = auxiliary \
	exception \
	commandline \
	structures \
	tagger \
	reader \
	writer

APPLIBS = auxiliary/*.o \
	exception/*.o \
	commandline/*.o \
	structures/*.o \
	tagger/*.o \
	reader/*.o \
	writer/*.o

all: applibs

applibs: subclean
	@for appdir in $(APPDIRS); do \
		(cd $${appdir} && $(MAKE) applib) || exit 1; \
	done
	g++ -std=c++11 -Wall -g -o tagit tagit.cpp $(APPLIBS) $(LIBS)
         
subclean:
	@for appdir in $(APPDIRS); do \
		(cd $${appdir} && $(MAKE) clean) || exit 1; \
	done
          
clean: subclean
	@-rm -f *.o
       