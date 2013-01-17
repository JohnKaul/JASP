#====================================================================
# Begin: Add your project information here
#====================================================================
#--------------------------------------------------------------------
# Set up the directory locations 
#--------------------------------------------------------------------
SRCDIR  	:= 	.
PROJDIR 	:= 	..
BINDIR  	:= 	$(PROJDIR)/release
INCDIR		:=	./include/
EXTRAINCDIR  	:= 	../../../include/
#
#--------------------------------------------------------------------
# Add your files here
#--------------------------------------------------------------------
SOURCES			:=	main.ls
# if you don't use spaces in your file names you can replace the above
# entry with the below entry (delete the line above and remove the '#'
# charater from the begining of the line). The entry below will build 
# all .ls files found in the directory.
#SOURCES	    	:=	$(notdir $(wildcard $(SRCDIR)/*.ls))

#--------------------------------------------------------------------
# possibly not necessary to edit below this line
#--------------------------------------------------------------------
CC      =	../../../../../bin/JASP
# processor
CFLAGS  =	-d __HTML -p -i $(INCDIR) -i $(EXTRAINCDIR)
# processor flags
SRC_EXT =	.ls
# source file ext
COBJ_EXT=	.html
# compiled source file ext
OBJECTS =	$(SOURCES:$(SRC_EXT)=$(COBJ_EXT))

#====================================================================
# Begin makefile
#====================================================================

all: $(SOURCES) $(OBJECTS)

.SUFFIXES: $(SRC_EXT) $(COBJ_EXT)

$(SRC_EXT)$(COBJ_EXT):
	@echo .
	@echo Processing File:  $<
	$(CC) $(CFLAGS) $< -o $(BINDIR)/$@

# vim: set noet set ff=dos
