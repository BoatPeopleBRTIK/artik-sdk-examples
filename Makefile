SUBDIRS := $(wildcard */.)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ all

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done

.PHONY: all $(SUBDIRS)
