SUBDIRS = rsMath Rgbhsl Implicit rsText rsXScreenSaver

all:
	@mkdir -p lib
	@for dir in $(SUBDIRS); do echo "Building $$dir..."; $(MAKE) -C $$dir || exit 1; done

clean:
	@for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean 2>/dev/null; done
	@rm -rf lib

.PHONY: all clean
