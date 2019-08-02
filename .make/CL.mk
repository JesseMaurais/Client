#define obj(src, ext) $(src:ext=.obj)
#define std(lang) -std:lang

append(CFLAGS, -nologo, -permissive- -EHsc)
#ifndef NDEBUG
append(CFLAGS, -Zi)
#endif
append(LDFLAGS, -nologo)

ANAL=-analyze
WARN=-W4 -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS
OUT=-Fe
