# Use line continuation to distinguish NMake from GNU
# \
!ifdef 0
DEPENDS=$(SRC:.cpp=.d)
-include $(DEPENDS)
# \
!endif
