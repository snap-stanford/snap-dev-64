#
# Makefile for non-Microsoft compilers
#

all: MakeAll

test: TestAll

MakeAll:
	$(MAKE) -C snap-core
	$(MAKE) -C examples

TestAll:
	$(MAKE) run -C test

core:
	$(MAKE) -C snap-core

clean:
	$(MAKE) clean -C snap-core
	$(MAKE) clean -C examples
	$(MAKE) clean -C test
	$(MAKE) clean -C tutorials
