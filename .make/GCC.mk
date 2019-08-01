#define obj(src, ext) $(src:ext=.o)
#define dep(src, ext) $(src:ext=.d)

WARN=-Wall -Wextra -Wpedantic
LDFLAGS=-rdynamic
FLAGS=-MP -MMD
USESTD=-std=
DEBUG=-g
OUT=-o 

