PKG_NAME=libcose
PKG_URL=https://github.com/bergzand/libcose
PKG_VERSION=63a46cc56bd735432356741864034e2977675b4f
PKG_LICENSE=LGPL

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.libcose
	"$(MAKE)" -C $(PKG_BUILDDIR)/src/crypt -f $(CURDIR)/Makefile.libcose_crypt

include $(RIOTBASE)/pkg/pkg.mk
