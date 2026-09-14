.PHONY: all flat

all: $(patsubst %.dot,%.svg,$(wildcard *.dot))

# the include tree with every include() resolved, checked for losslessness
flat:
	../tools/cmake-flatten.py --verify

%.svg:	%.dot
	dot -Tsvg -o $@ $<
