#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
define newline


endef

SUBDIRS := busk usk
#$(info $$SUBDIRS is [${SUBDIRS}])

.PHONY: all prepare subdirs clean $(SUBDIRS)

#---------------------------------------------------------------------------------
all: prepare

prepare: subdirs
	python3 usk/prepare.py busk/cmake-build-release/busk.bin usk/cmake-build-release/usk.bin

subdirs: $(SUBDIRS)
	@mkdir -p usk/generated
	@$(foreach d,$(SUBDIRS),$(MAKE) -C $(d) || true $(newline))

clean:
	@$(foreach d,$(SUBDIRS),$(MAKE) -C $(d) clean || true $(newline))
	@rm -f firmware.uf2 update.bin
