# Use line continuation to distinguish NMake from GNU
# \
include .make/NMake.mk
# \
!ifdef 0
include .make/GNU.mk
# \
!endif
include .make/Cppcheck.mk
include .make/PVS.mk
