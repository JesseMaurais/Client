#define obj(src, ext) $(src:ext=.o)
#define dep(src, ext) $(src:ext=.d)
#define std(lang) -std=lang

append(CFLAGS, -MP -MMD)
#ifndef NDEBUG
append(CFLAGS, -g)
#endif
append(LDFLAGS, -rdynamic)

WARN=-Wall -Wextra -Wpedantic
OUT=-o 

