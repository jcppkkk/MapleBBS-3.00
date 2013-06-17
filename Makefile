# ------------------------------------------------------- #
#  util/Makefile	( NTHU CS MapleBBS Ver 2.36 )   	  #
# ------------------------------------------------------- #
#  target : Makefile for ALL				  #
# ------------------------------------------------------- #
include include.mk
DIRS += lib
DIRS += WD
DIRS += SO
DIRS += util
BUILDDIRS = $(DIRS:%=build-%)
INSTALLDIRS = $(DIRS:%=install-%)
CLEANDIRS = $(DIRS:%=clean-%)

all: $(BUILDDIRS)
	$(MAKE) -C innbbsd $(OSTYPE)

$(DIRS): $(BUILDDIRS)
$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%)

# install subdir
install: $(INSTALLDIRS) all
	$(MAKE) -C innbbsd $(OSTYPE) install
$(INSTALLDIRS):
	$(MAKE) -C $(@:install-%=%) install

# clean subdir
clean: $(CLEANDIRS) 
	make -C innbbsd clean
	$(CLEANDIRS): 
	$(MAKE) -C $(@:clean-%=%) clean
