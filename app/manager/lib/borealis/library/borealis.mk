mkfile_path	:=	$(abspath $(lastword $(MAKEFILE_LIST)))
current_dir	:=	$(BOREALIS_PATH)/$(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
$(info $$current_dir is [${current_dir}])

LIBS		:=	-lglfw3 -lEGL -lglad -lglapi -ldrm_nouveau -lm $(LIBS)

# fmt wrapper is there because dkp's base
# makefile doesn't recognize .cc files as c++

#SOURCES		:=	$(SOURCES) \
#				$(current_dir)/lib \
#				$(current_dir)/lib/extern/nanovg-gl \
#				$(current_dir)/lib/extern/libretro-common/compat \
#				$(current_dir)/lib/extern/libretro-common/encodings \
#				$(current_dir)/lib/extern/libretro-common/features \
#				$(current_dir)/lib/extern/nxfmtwrapper
#
#INCLUDES	:=	$(INCLUDES) \
#				$(current_dir)/include \
#				$(current_dir)/lib/extern/fmt/include \
#				$(current_dir)/include/borealis/extern

SOURCES		:=	$(SOURCES) \
        ${current_dir}/lib/core \
        ${current_dir}/lib/core/touch \
        ${current_dir}/lib/views \
        ${current_dir}/lib/views/cells \
        ${current_dir}/lib/views/widgets \
        ${current_dir}/lib/extern/glad \
        ${current_dir}/lib/extern/libretro-common/compat \
        ${current_dir}/lib/extern/libretro-common/encodings \
        ${current_dir}/lib/extern/libretro-common/features \
        ${current_dir}/lib/extern/nanovg-gl \
        ${current_dir}/lib/extern/yoga/yoga \
        ${current_dir}/lib/extern/yoga/yoga/event \
        ${current_dir}/lib/extern/fmt/src \
  		${current_dir}/lib/extern/tinyxml2 \
		${current_dir}/lib/platforms/switch \
		${current_dir}/lib/extern/switch-libpulsar/src/archive \
		${current_dir}/lib/extern/switch-libpulsar/src/bfgrp \
		${current_dir}/lib/extern/switch-libpulsar/src/bfsar \
		${current_dir}/lib/extern/switch-libpulsar/src/bfwar \
		${current_dir}/lib/extern/switch-libpulsar/src/bfwav \
		${current_dir}/lib/extern/switch-libpulsar/src/bfwsd \
		${current_dir}/lib/extern/switch-libpulsar/src/player

INCLUDES	:=	$(INCLUDES) \
        ${current_dir}/include \
        ${current_dir}/include/borealis/extern \
        ${current_dir}/include/borealis/extern/nanovg-gl \
	    ${current_dir}/include/borealis/extern/tinyxml2 \
		${current_dir}/lib/extern/switch-libpulsar/include \
	    ${current_dir}/lib/extern/yoga \
	    ${current_dir}/lib/extern/tweeny/include \
	    ${current_dir}/lib/extern/fmt/include
