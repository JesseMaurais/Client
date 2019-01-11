# Load the project configurations
.include ".make/Configure.mk"

# Guess the environment
.ifdef COMSPEC
# Probably Windows
.include ".make/CMD.mk"
.elifdef SHELL
# Probably UNIX
.include ".make/SH.mk"
.else
# Bad way
$(error Cannot determine the operating environment)
.endif
.endif

# Guess the compiler used
.ifneq ${CXX} == ${CXX:clang=}
.include ".make/Clang.mk"
.else
.ifneq ${CXX} == ${CXX:g++=}
.include ".make/GCC.mk"
.else
.ifneq ${CXX} == ${CXX:cl=}
.include ".make/CL.mk"
.else
$(warning Cannot determine your compiler flags)
.endif # CL
.endif # GCC
.endif # Clang
