
CxPfx = ../../cx

all: \
	gi.pdf \
	polystep.pdf \
	valuations.pdf

%.pdf: %.tex $(addprefix $(CxPfx)/doc/,preamble.tex predef.tex)
	env TEXINPUTS=$(CxPfx)/doc:.: pdflatex $<

.PHONY: clean
clean:
	rm -f *.aux *.log

.PHONY: distclean
distclean: clean
	rm -f *.pdf

