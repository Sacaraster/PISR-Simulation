
# root is relative from inside the created subdirectories in graphsim/linux/
# for example: graphsim/linux/gsim/  etc
export ROOT = ../..
export INCLUDEDIR = -I$(ROOT)/ 
export LIBDIR = -L $(ROOT)/lib 
export LIBS = -lgsimtripath -lglut -lGL -lGLU 

# listed here are the names of the modules to be compiled
DIRS = gsimtripath setut

export CC = g++
export CFLAGS = $(INCLUDEDIR) -D GS_SYSTEM_RAND_LIBS
export LFLAGS = $(LIBDIR) $(LIBS)

# Be quiet when building...
.SILENT:

all: 
	@for dir in $(DIRS); do\
		echo "=== making $$dir ===";\
		mkdir -p $$dir; \
		cd $$dir; \
		$(MAKE) -f ../makefile.$$dir;\
		cd ..; \
	done
	echo "=== done ===";

clean:
	mv ../bin/readme.txt ./binreadmetmp
	$(RM) core *.o *~ ../lib/*.a ../bin/*
	mv ./binreadmetmp ../bin/readme.txt
	@for dir in $(DIRS); do \
		$(RM) -f -r $$dir; \
	done


