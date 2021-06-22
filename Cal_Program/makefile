## Calibration Execution Makefile

#defines targets all and clean
.PHONY = all clean

#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#
#	$^ The names of all the prerequisites, with spaces between them.
#	$< The name of the first prerequisite.
#	$@ The file name of the target of the rule

# define the compiler to use
CC = g++

# compile-time flags
#CFLAGS =-Wall -g -fsanitize=address -static-libasan -fsanitize=leak -fno-omit-frame-pointer
CFLAGS =-g -fsanitize=address -static-libasan -fsanitize=leak -fno-omit-frame-pointer
	# -g provides for debugging with gdb
	# -Wall provides extra errors and warnings from compilation
	# -fsanitize=leak catches memory leaks, out of bound array access, and use after free errors

UTIL_DIR =./src/Util/
YOKOGAWA_DIR =./src/Yokogawa/
PI_DIR =./src/PI/
MAIN_DIR =./src/
E3648A_DIR = ./src/E3648A
HP34401_DIR = ./src/HP34401/
CREATE_LUT_DIR = ./src/Create_LUT/
LUT_DIR = ./src/LUT
SSH_DIR =./src/SSH/
CONFIG_DIR = ./src/Configure/

# define any include directories for header files other than /usr/include
# -I means include directory
INCLUDES = -I $(HP34401_DIR) -I $(E3648A_DIR) -I $(YOKOGAWA_DIR) -I $(UTIL_DIR) -I $(PI_DIR) -I $(MAIN_DIR) -I $(LUT_DIR) -I $(CREATE_LUT_DIR) \
			-I $(CONFIG_DIR) -I $(SSH_DIR) -I ./
E3648_INCLUDE = -I /src/E3648A/E36468.h

# libraries to link into executable:
# link in libbcm2835.so and libm.so: (PI GPIO library and math library)
#LIBS = -l bcm2835 -l m
LIBS =-l bcm2835 -l m -l ssh

# define the source .cpp files
YOKOGAWA_SRC = $(wildcard src/Yokogawa/*.cpp) 
PI_SRC = $(wildcard src/PI/*.cpp)
UTIL_SRC = $(wildcard src/Util/*.cpp)
MAIN_SRC = $(wildcard src/*.cpp)
LUT_SRC = $(wildcard src/LUT/*.cpp)
CREATE_LUT_SRC = $(wildcard src/Create_LUT/*.cpp)
SSH_SRC = $(wildcard src/SSH/*.cpp)
CONFIG_SRC = $(wildcard src/Configure/*.cpp)


PARAMETERS_DEF_FILE = ./Cal_Parameters.cpp
SRCS = $(YOKOGAWA_SRC) $(PI_SRC) $(UTIL_SRC) $(MAIN_SRC) $(CREATE_LUT_SRC) $(SSH_SRC) $(CONFIG_SRC)

UTIL_INCLUDES = 	-I /src/Util/Struct_Defs.h \
					-I /src/Util/TStamp.h \
					-I /src/Util/TimeH.h \
					-I /src/Util/StringH.h \
					-I /src/Util/Data.h \
					

# define the name of the executable file 
MAIN = cal

OBJ_DIR := obj
BIN_DIR := bin
SRC_DIR := src

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of the rule (a .c file)
#  $@: the name of the target of the rule (a .o file) 
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

YOKOGAWA_OBJ = $(YOKOGAWA_SRC:.cpp=.o)
PI_OBJ = $(PI_SRC:.cpp=.o)
UTIL_OBJ = $(UTIL_SRC:.cpp=.o)
MAIN_OBJ = $(MAIN_SRC:.cpp=.o) 
LUT_OBJ = $(LUT_SRC:.cpp=.o)
CREATE_LUT_OBJ = $(CREATE_LUT_SRC:.cpp=.o)
SSH_OBJ = $(SSH_SRC:.cpp=.o)
PARAMETERS_OBJ = $(PARAMETERS_DEF_FILE:.cpp=.o) 
CONFIG_OBJ = $(CONFIG_SRC:.cpp=.o)
OBJS = $(YOKOGAWA_OBJ) $(PI_OBJ) $(UTIL_OBJ) $(MAIN_OBJ) $(PARAMETERS_OBJ) $(LUT_OBJ) $(CREATE_LUT_OBJ) $(SSH_OBJ) $(CONFIG_OBJ)

OBJECT_NAMES = $(notdir $(OBJS))
OBJ = $(addprefix $(OBJ_DIR)/, $(OBJECT_NAMES) )

ccred=$(shell echo -e "\033[0;31m")
ccend=$(shell echo -e "\033[0m")

red:=$(shell tput setaf 1)
bold:=$(shell tput bold)
yellow:=$(shell tput setaf 3)
reset:=$(shell tput sgr0)

#OBJ := $(patsubst %,$(OBJ_DIR)%,$(OBJ1))
all: target

# the $(OBJ_DIR) and $(BIN_DIR) creates those directories if they dont already exist
target: object_files
	@cp $(MAIN) $(BIN_DIR)/$(MAIN)

object_files: $(OBJ_DIR) $(BIN_DIR) lut_module ssh_module util_module yokogawa_module pi_module parameters create_lut_module config_module cal_main 
	@echo "$(yellow) Linking.... $(reset)"
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJ) $(LIBS)
	@echo '-------------------------------------------------------------------'
	@echo 'done.'
	@echo 'created executable  "$(bold)$(red)$(MAIN)$(reset)"'
	@echo 'to run, enter command:  $(bold)$(red) ./$(MAIN)$(reset) (do not use sudo)'

# for all the below rules, the object files are created in the current working directory.
# so the mv command after the compile line moves the object files to the $(OBJ_DIR)
cal_main:
	@echo "$(yellow) Cal Main Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) $(E3648_INCLUDE) -c $(MAIN_SRC) $(PARAMETERS_DEF_FILE) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

create_lut_module:
	@echo "$(yellow) LUT Filter and Creation Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(CREATE_LUT_SRC) $(PARAMETERS_DEF_FILE) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

util_module:
	@echo "$(yellow) Util Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(UTIL_SRC) $(PARAMETERS_DEF_FILE) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

lut_module:
	@echo "$(yellow) LUT Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(LUT_SRC) $(PARAMETERS_DEF_FILE) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

yokogawa_module: 
	@echo "$(yellow) Yokogawa Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(YOKOGAWA_SRC) $(UTIL_INCLUDES) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

pi_module:
	@echo "$(yellow) PI Module $(reset)" 
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(PI_SRC) $(PARAMETERS_DEF_FILE) $(UTIL_INCLUDES) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

ssh_module:
	@echo "$(yellow) SSH Module $(reset)" 
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(SSH_SRC) $(PARAMETERS_DEF_FILE) $(UTIL_INCLUDES) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

config_module:
	@echo "$(yellow) Configure Module $(reset)" 
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(CONFIG_SRC) $(PARAMETERS_DEF_FILE) $(UTIL_INCLUDES) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

parameters:
	@echo "$(yellow) Parameters Module $(reset)"
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(PARAMETERS_DEF_FILE) $(UTIL_INCLUDES) $(LIBS)
	@mv *.o $(OBJ_DIR)
	@echo ""

$(OBJ_DIR):
	@mkdir -p $@

$(BIN_DIR):
	@mkdir -p $@
clean:
	$(RM) *.o *~ $(MAIN)
	$(RM) $(OBJ_DIR)/*.o
	$(RM) $(BIN_DIR)/*










