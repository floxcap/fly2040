#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

# recursive wildcard
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# ROMFS is the directory containing data to be added to RomFS, relative to the Makefile (Optional)
#
# NO_ICON: if set to anything, do not use icon.
# NO_NACP: if set to anything, no .nacp file is generated.
# APP_TITLE is the name of the app stored in the .nacp file (Optional)
# APP_AUTHOR is the author of the app stored in the .nacp file (Optional)
# APP_VERSION is the version of the app stored in the .nacp file (Optional)
# APP_TITLEID is the titleID of the app stored in the .nacp file (Optional)
# ICON is the filename of the icon (.jpg), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.jpg
#     - icon.jpg
#     - <libnx folder>/default_icon.jpg
#
# CONFIG_JSON is the filename of the NPDM config file (.json), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.json
#     - config.json
#   If a JSON file is provided or autodetected, an ExeFS PFS0 (.nsp) is built instead
#   of a homebrew executable (.nro). This is intended to be used for sysmodules.
#   NACP building is skipped as well.
#---------------------------------------------------------------------------------
TARGET		:=	sys-rgb-manager
BUILD		:=	build.nx
SOURCES		:=	src ../common/src ../common/src/client
RESOURCES	:=	resources
DATA		:=	data
INCLUDES	:=	../common/include
APP_TITLE	:=	sys-rgb Manager
APP_AUTHOR	:=	Floxcap
ICON		:=  resources/icon/icon.jpg

ROMFS				:=	$(BUILD)/romfs
BOREALIS_PATH		:=	lib/borealis
BOREALIS_RESOURCES	:=	romfs:/
APP_RESOURCES		:=	romfs:/

#---------------------------------------------------------------------------------
# version control constants
#---------------------------------------------------------------------------------
TARGET_VERSION	:= $(shell git describe --dirty --always --tags)
APP_VERSION		:= $(TARGET_VERSION)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
DEFINES	+=	-DDISABLE_IPC -DJSON_NOEXCEPTION -DTARGET="\"$(TARGET)\"" -DTARGET_VERSION="\"$(TARGET_VERSION)\""

ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS	:=	-g -Wall -O2 -ffunction-sections -fno-exceptions $(ARCH) $(DEFINES)

CFLAGS	+=	$(INCLUDE) -D__SWITCH__ \
			-DAPP_TITLE="\"$(APP_TITLE)\"" \
			-DBOREALIS_RESOURCES="\"$(BOREALIS_RESOURCES)\"" \
			-DBRLS_RESOURCES="\"$(BOREALIS_RESOURCES)\"" \
			-DAPP_RESOURCES="\"$(APP_RESOURCES)\"" \
			-DBOREALIS_USE_OPENGL \
			-DYG_ENABLE_EVENTS \
			-D__GLFW__ \
			-DSTBI_NO_THREAD_LOCALS

CXXFLAGS := $(CFLAGS) -std=gnu++20

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lnx -lm $(foreach lib,$(LIBNAMES),-l$(lib))

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(LIBNX) $(foreach lib,$(LIBNAMES),$(TOPDIR)/lib/$(lib))
#$(info $$LIBDIRS is [${LIBDIRS}])

LIBBLDDIRS := $(foreach lib,$(LIBNAMES),$(TOPDIR)/lib/$(lib))
#$(info $$LIBBLDDIRS is [${LIBBLDDIRS}])

include $(TOPDIR)/$(BOREALIS_PATH)/library/borealis.mk

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))
export VPATH += $(TOPDIR)/$(BOREALIS_PATH)/library/lib/platforms/glfw/

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
CPPFILES	+=	glfw_video.cpp
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

ifeq ($(strip $(CONFIG_JSON)),)
	jsons := $(wildcard *.json)
	ifneq (,$(findstring $(TARGET).json,$(jsons)))
		export APP_JSON := $(TOPDIR)/$(TARGET).json
	else
		ifneq (,$(findstring config.json,$(jsons)))
			export APP_JSON := $(TOPDIR)/config.json
		endif
	endif
else
	export APP_JSON := $(TOPDIR)/$(CONFIG_JSON)
endif

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(CURDIR)/$(TARGET).nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif

.PHONY: $(BUILD) clean clean-libs clean-build all libs

#---------------------------------------------------------------------------------
all: $(BUILD)

libs:
	@$(foreach lib,$(LIBBLDDIRS),$(MAKE) --no-print-directory -C $(lib) && ) true

$(LIBNAMES):
	@echo Building $@

$(ROMFS):
	@[ -d $@ ] || mkdir -p $@
	@echo Merging ROMFS...
	@rsync -a $(CURDIR)/$(RESOURCES)/ $(CURDIR)/$(ROMFS)/

	#@cp -ruf $(CURDIR)/$(BOREALIS_PATH)/resources/. $(CURDIR)/$(ROMFS)/borealis/

$(BUILD): $(ROMFS) libs
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean-libs:
	@echo clean libs $(LIBNAMES) ...
	@$(foreach lib,$(LIBBLDDIRS),$(MAKE) -C $(lib) clean && ) true

clean-build:
	@echo clean build ...
	@rm -fr $(BUILD) $(TARGET).kip $(TARGET).nsp $(TARGET).npdm $(TARGET).nso $(TARGET).elf $(OUTDIR)

clean: clean-libs clean-build
	@echo clean ...
ifeq ($(strip $(APP_JSON)),)
	@rm -fr $(BUILD) $(TARGET).nro $(TARGET).nacp $(TARGET).elf
else
	@rm -fr $(BUILD) $(TARGET).nsp $(TARGET).nso $(TARGET).npdm $(TARGET).elf
endif
	@mkdir -p $(BUILD)
	@mkdir -p $(ROMFS)


#---------------------------------------------------------------------------------
else
.PHONY:	all $(LIBFILES)

LIBFILES := $(foreach lib,$(LIBNAMES),$(call rwildcard,$(subst lib/stratosphere,../common/lib/Atmosphere-libs/libstratosphere,$(TOPDIR)/lib/$(lib)/lib),*.a))
#$(info $$LIBFILES is [${LIBFILES}])

export LIBPATHS	:= $(foreach dir,$(sort $(dir $(foreach lib,$(LIBDIRS),$(call rwildcard,$(lib)/lib,*.a)))),-L$(dir))
#$(info $$LIBPATHS is [${LIBPATHS}])

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
ifeq ($(strip $(APP_JSON)),)

all	:	$(OUTPUT).nro

ifeq ($(strip $(NO_NACP)),)
$(OUTPUT).nro	:	$(OUTPUT).elf $(OUTPUT).nacp
else
$(OUTPUT).nro	:	$(OUTPUT).elf
endif

else

all	:	$(OUTPUT).nsp

$(OUTPUT).nsp	:	$(OUTPUT).nso $(OUTPUT).npdm

$(OUTPUT).nso	:	$(OUTPUT).elf

endif

$(OUTPUT).elf	:	$(OFILES) $(LIBFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
