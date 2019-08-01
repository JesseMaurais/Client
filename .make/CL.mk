// Configurations for the Microsoft CL compiler usually used in Visual C++

#define obj(src, ext) $(src:ext=.obj)

ANAL=-analyze
WARN=-W4 -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS
LDFLAGS=-nologo 
FLAGS=-nologo -permissive- -EHsc
USESTD=-std:
DEBUG=-Zi
OUT=-Fe
