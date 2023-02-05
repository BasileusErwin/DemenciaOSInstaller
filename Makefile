# Compiler settings
CC = g++
CXXFLAGS = -std=c++17 -Wall -O2
LDFLAGS = -lncurses -lparted

# Makefile settings
APPNAME = DemenciaOSInstaller
EXT = .cpp
SRCDIR = .
OBJDIR = .

# For no install version use:
# make CXXFLAGS=-DNOINSTALL

############## Do not change anything from here downwards! #############
SRC = $(wildcard $(SRCDIR)/*$(EXT))
OBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o)
DEP = $(OBJ:$(OBJDIR)/%.o=%.d)
RM = rm
DELOBJ = $(OBJ)

all: $(APPNAME)

# Builds the app
$(APPNAME): $(OBJ)
	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Creates the dependecy rules
%.d: $(SRCDIR)/%$(EXT)
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:%.d=$(OBJDIR)/%.o) >$@

# Includes all .h files
-include $(DEP)

# Building rule for .o files and its .c/.cpp in combination with all .h
$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT)
	$(CC) $(CXXFLAGS) -o $@ -c $<

# Cleans complete project
.PHONY: clean
clean:
	@$(RM) $(DELOBJ) $(DEP) $(APPNAME)

# Cleans only all files with the extension .d
.PHONY: cleandep
cleandep:
	@$(RM) $(DEP)