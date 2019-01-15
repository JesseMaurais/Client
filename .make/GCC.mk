# Configurations for the GNU C++ compiler usually used on Linux
WARN = -Wall -Wextra -Wpedantic
FLAGS = -MP -MMD
USESTD = -std=
DEBUG = -g
OUT = -o 
OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)
