#### Start of system configuration section ####

VERSION := 1.00
CC      := gcc
AR      := ar
CFLAGS  := -Wall
AFLAGS  := rcs
RM      := rm

#### End of system configuration section ####

# Path for .c , .h and .o Files 
SRC_DIR = src
HDR_DIR = src
OBJ_DIR = obj
LIB_DIR = lib
TST_DIR = examples

# name of the library
LIB_NAME  := rlib

# cleaning settings
RM = rm		# unix cleaning
DEL = del	# windows cleaning

ifeq ($(OS),Windows_NT)
EXE = .exe
else
EXE = 
endif

### Start of all necessary files ###

# .c files
LIB_SRC := $(wildcard $(SRC_DIR)/*.c)
TST_SRC := $(wildcard $(TST_DIR)/*.c)
# .c files without the directory
LIB_NODIR := $(notdir $(LIB_SRC))
TST_NODIR := $(notdir $(TST_SRC))
# .o files
LIB_OBJ := $(addprefix $(OBJ_DIR)/,$(addprefix $(SRC_DIR)_,$(notdir $(LIB_SRC:.c=.o))))
TST_OBJ := $(addprefix $(OBJ_DIR)/,$(addprefix $(TST_DIR)_,$(notdir $(TST_SRC:.c=.o))))
# .a file
LIB_OUT  := $(LIB_DIR)/lib$(LIB_NAME).a
# output files
TST_OUT := $(TST_NODIR:.c=)
# TST_EXE := $(addsuffix $(EXE),$(TST_NODIR:.c=))

# cleaning objects
# WDELOBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)\\%.o)

### End of all necessary files ###

# 'make' creates both examples and the library
# 'make examples' creates examples
# 'make [insert filename without extension from examples directory]' creates only one example
examples: $(TST_OUT)
# 'make lib' creates the library
lib: $(LIB_OUT)

### Start of building rules ###

# create output files
$(TST_OUT): $(LIB_OUT)
	$(CC) -o $@ $(CFLAGS) $(TST_DIR)/$@.c -L$(LIB_DIR) -l$(LIB_NAME)

# link .o files into static libraries	
$(LIB_OUT): $(LIB_OBJ)
	$(AR) $(AFLAGS) $(LIB_OUT) $^

# compile .c files into object files
$(OBJ_DIR)/$(SRC_DIR)_%.o: $(SRC_DIR)/%.c $(HDR_DIR)/%.h
	$(CC) $(CFLAGS) -c -o $@ $<

### End of building rules ###

# # Cleans complete project
# .PHONY: clean

# ifeq ($(OS),Windows_NT)
# #################### Cleaning rules for Windows OS #####################
# clean:
# 	$(DEL) $(LIB_OBJ) $(DEP) $(APPNAME)$(EXE)
# else
# ################### Cleaning rules for Unix-based OS ###################
# clean:
# 	$(RM) $(LIB_OBJ) $(DEP) $(APPNAME)
# endif
