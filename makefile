COMPILER=g++
CPPFLAGS=-w -Wall
LINKER=@g++
LINKERFLAGS=

PUBLIC_DIR=/public/tp/tp-multitache/
INCPATH=-I $(PUBLIC_DIR)
LIBPATH=-L $(PUBLIC_DIR)
# In case the public dir is not mounted at the same point
INCPATH+= -I /shares$(PUBLIC_DIR)
LIBPATH+= -L /shares$(PUBLIC_DIR)
LIBS=-lcurses -ltcl -ltp

ECHO=@echo
CLEAN=clean

SRCDIR=src
OUTPUTDIR=bin
EXE=Parking

FILES= common.h KeyboardManagement.h Entrance.h ExitGate.h

HEADERS=$(addprefix $(SRCDIR)/,$(FILES))
IMPL=$(HEADERS:.h=.cpp)

OBJ=$(addprefix $(OUTPUTDIR)/,$(FILES:.h=.o))
OBJ+=$(OUTPUTDIR)/main.o

.PHONY: all before $(CLEAN)

all: before $(EXE)

before:
	if [[ ! -d bin ]]; then mkdir bin; fi

# Ouput executable
$(EXE): $(OBJ)
	$(ECHO) Linking $(EXE)...
	$(LINKER) $(LINKERFLAGS) $(INCPATH) $(LIBPATH) -o $(OUTPUTDIR)/$(EXE) $(OBJ) $(LIBS)

# Generic rule
$(OUTPUTDIR)/%.o: $(SRCDIR)/%.cpp $(SRCDIR)/common.h
	$(ECHO) Compiling $<...
	$(COMPILER) $(CPPFLAGS) $(INCPATH) -o $@ -c $<

# Explicit dependancies


$(CLEAN):
	$(ECHO) Cleaning project.
	rm -f $(OBJ) $(OUTPUTDIR)/$(EXE) core
