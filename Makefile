
BldPath=bld
TopBldPath=$(BldPath)/ext
SrcPath=src
DepPath=dep
CxPath=$(DepPath)/cx

CMAKE=cmake
GODO=$(CMAKE) -E chdir
MKDIR=$(CMAKE) -E make_directory

.PHONY: default
default:
	if [ ! -d $(TopBldPath) ] ; then $(MAKE) cmake ; fi
	$(MAKE) proj

.PHONY: all
all:
	$(MAKE) cmake
	$(MAKE) proj

.PHONY: cmake
cmake:
	if [ ! -d $(BldPath) ] ; then $(MKDIR) $(BldPath) ; fi
	if [ ! -d $(TopBldPath) ] ; then $(MKDIR) $(TopBldPath) ; fi
	$(GODO) $(TopBldPath) $(CMAKE) ../..

.PHONY: proj
proj:
	$(GODO) $(TopBldPath) $(MAKE)

.PHONY: test
test:
	$(GODO) $(BldPath) $(MAKE) test

.PHONY: clean
clean:
	$(GODO) $(TopBldPath) $(MAKE) clean

.PHONY: init
init:
	if [ ! -f $(CxPath)/cx.c ] ; then git submodule init dep/cx ; fi
	if [ ! -f $(CxPath)-pp/cx.c ] ; then git submodule init dep/cx-pp ; fi

.PHONY: update
update:
	git pull
	git submodule update
	git submodule foreach git checkout master
	git submodule foreach git merge --ff-only origin/master

