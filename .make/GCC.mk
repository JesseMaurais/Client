# Configurations for the GNU C++ compiler usually used on Linux
CFLAGS += -MP -MMD -g -std=$(STD)
COUT = -o
OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)
