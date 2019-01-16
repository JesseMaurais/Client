# Configurations for the Microsoft CL compiler usually used in Visual C++
ANALYZE = -analyze
WARN = -W4 -D_CRT_SECURE_NO_WARNINGS # -Wall gives too many warnings in system headers
FLAGS = -nologo -permissive- -EHsc
USESTD = -std:
DEBUG = -Z7
OUT = -Fe
OBJ = $(SRC:.cpp=.obj)
