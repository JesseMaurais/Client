#define exe(x) x
#define dll(x) x.so
#define lib(x) lib##x.a

append(CFLAGS, -D_POSIX_C_SOURCE)
append(LNK, -ldl -lrt -lpthread)

