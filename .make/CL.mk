# Configurations for the Microsoft CL compiler usually used in Visual C++
WARN = -Wall
FLAGS = -nologo -permissive- -EHsc
USESTD = -std:
DEBUG = -Z7
OUT = -Fe
OBJ = $(SRC:.cpp=.obj)
