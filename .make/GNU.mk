
ifdef STD
override CFLAGS += -std=$(STD)
endif

override CFLAGS += -g -MP -MMD
COUT = -o 
RM = rm -f
