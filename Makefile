#################################################
#            Variables                          #
#################################################

# Directories
SRC = ./

# SET DEFAULTS (which can be overriden)
ifeq ($(origin CXX), undefined)
	CXX = g++
endif

ifeq ($(origin LDFLAGS), undefined)
	LDFLAGS = -g -Wall -O0 -lserial
endif

ifeq ($(origin CXXFLAGS), undefined)
	CXXFLAGS = $(LDFLAGS) -DDEBUG=1 -MD # MD is required for auto dependency
endif

#################################################
#                    modules                    #
#################################################

# COMMON OBJECT FILES
COMMONOBJS = $(SRC)sal.o $(SRC)Workload.o $(SRC)WattsUp.o $(SRC)Diskstats.o $(SRC)CPUstats.o $(SRC)Log.o

# COMPILATION RULES
###################

sal: $(COMMONOBJS)
	$(CXX) -o $@ $(COMMONOBJS) $(CXXFLAGS) $(INC) $(LDFLAGS) -lm

# GENERIIC COMPILATION RULE
.C.o:
	$(CXX) $< -c $(CXXFLAGS) $(INC)
	
WattsUp: WattsUp.cpp WattsUp.h
	g++ -Wall -g -o WattsUp WattsUp.cpp	-lserial -DDEBUG

# AUTOMATIC DEPENDENCY DETECTION
# http://www.wlug.org.nz/MakefileHowto
# also take a look at http://lear.inrialpes.fr/people/klaeser/software_makefile_link_dependencies

DEPS := $(patsubst %.o,%.d,$(COMMONOBJS))

-include $(DEPS)

# It might also help to make a 'deps' target:

#deps: $(ANCOBJS)
#	$(CXX) -M $(ANCOBJS:.o=.cc) > /dev/null

#################################################
#                  Clean                        #
#################################################

clean:
	rm -rf *.o *.d