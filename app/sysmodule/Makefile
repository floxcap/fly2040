#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules
include $(TOPDIR)/lib/Atmosphere-libs/config/templates/stratosphere.mk

# recursive wildcard
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# EXEFS_SRC is the optional input directory containing data copied into exefs, if anything this normally should only contain "main.npdm".
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
TARGET		:=	sys-rgb
BUILD		:=	build
OUTDIR		:=	out
RESOURCES	:=	res
SOURCES		:=	src ../common/src
DATA		:=	data
INCLUDES	:=	../common/include lib/Atmosphere-libs/libvapours/include lib/Atmosphere-libs/libstratosphere/include
EXEFS_SRC	:=	exefs_src
LIBNAMES	:=	stratosphere
NO_ICON		:=  1

#---------------------------------------------------------------------------------
# version control constants
#---------------------------------------------------------------------------------
TARGET_VERSION	:= $(shell git describe --dirty --always --tags)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
DGB_FLAG =

DEFINES	+=	-DDISABLE_IPC -DJSON_NOEXCEPTION -DTARGET="\"$(TARGET)\"" -DTARGET_VERSION="\"$(TARGET_VERSION)\""

ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS	:=	$(DGB_FLAG) -Wall -O2 -flto -ffunction-sections -fno-exceptions $(ARCH) $(DEFINES)

CFLAGS	+=	$(INCLUDE) -D__SWITCH__

CXXFLAGS := $(CFLAGS) -fno-rtti -std=gnu++20

ASFLAGS	:=	$(DGB_FLAG) $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs $(DGB_FLAG) $(ARCH) -Wl,-Map,$(notdir $*.map) -flto -O2

LIBS	:= -lnx $(foreach lib,$(LIBNAMES),-l$(lib))
#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(LIBNX) $(foreach lib,$(LIBNAMES),$(TOPDIR)/lib/$(lib))
LIBDIRS	:= $(subst lib/stratosphere,lib/Atmosphere-libs/libstratosphere,$(LIBDIRS))
#$(info $$LIBDIRS is [${LIBDIRS}])

LIBBLDDIRS := $(subst lib/stratosphere,lib/Atmosphere-libs/libstratosphere,$(foreach lib,$(LIBNAMES),$(TOPDIR)/lib/$(lib)))
#$(info $$LIBBLDDIRS is [${LIBBLDDIRS}])

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(OUTDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
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

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export BUILD_EXEFS_SRC := $(TOPDIR)/$(EXEFS_SRC)

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

.PHONY: $(BUILD) clean clean-libs clean-build all libs dirs

#---------------------------------------------------------------------------------
all: $(BUILD)

libs:
	@$(foreach lib,$(LIBBLDDIRS),$(MAKE) --no-print-directory -C $(lib) && ) true

$(LIBNAMES):
	@echo Building $@

dirs:
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)
	@[ -d $(OUTDIR) ] || mkdir -p $(OUTDIR)

$(BUILD): libs dirs
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean-libs:
	@echo clean libs $(LIBNAMES) ...
	@$(foreach lib,$(LIBBLDDIRS),$(MAKE) -C $(lib) clean && ) true

clean-build:
	@echo clean build ...
	@rm -fr $(BUILD) $(TARGET).kip $(TARGET).nsp $(TARGET).npdm $(TARGET).nso $(TARGET).elf $(OUTDIR)

clean: clean-libs clean-build
	$(MAKE) dirs


#---------------------------------------------------------------------------------
else
.PHONY:	all $(LIBFILES)

LIBFILES := $(foreach lib,$(LIBNAMES),$(call rwildcard,$(subst lib/stratosphere,lib/Atmosphere-libs/libstratosphere,$(TOPDIR)/lib/$(lib)/lib),*.a))
#$(info $$LIBFILES is [${LIBFILES}])

export LIBPATHS	:= $(foreach dir,$(sort $(dir $(foreach lib,$(LIBDIRS),$(call rwildcard,$(lib)/lib,*.a)))),-L$(dir))
#$(info $$LIBPATHS is [${LIBPATHS}])

DEPENDS		:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

all: $(OUTPUT).nsp

$(OUTPUT).nsp: $(OUTPUT).nso $(OUTPUT).npdm

$(OUTPUT).elf: $(OFILES) $(LIBFILES)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
