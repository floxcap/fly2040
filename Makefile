#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
define newline


endef

# recursive wildcard
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

ifeq (, $(shell which aarch64-none-elf-g++))
 export PATH := /opt/devkitpro/devkitA64/bin/:${PATH}
endif

# app/overlay
SUBDIRS := app/manager app/sysmodule firmware
$(info $$SUBDIRS is [${SUBDIRS}])

.PHONY: all subdirs clean $(SUBDIRS)

#---------------------------------------------------------------------------------
all: subdirs

subdirs: $(SUBDIRS)
	@$(foreach d,$(SUBDIRS),$(MAKE) -C $(d) || true $(newline))

clean:
	@$(foreach d,$(SUBDIRS),$(MAKE) -C $(d) clean || true $(newline))
