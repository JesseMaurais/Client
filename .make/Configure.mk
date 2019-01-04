# Designed to work with both NMake and GNU make.
# Using line continuation to include the correct configuration.
# \
!ifdef 0 
include .make/GNU.mk
# \
!endif
# \
include NMake.mk
