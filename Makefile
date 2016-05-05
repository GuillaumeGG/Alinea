PACKAGE = 'minicas'
VERSION = '0.42.0'

PREFIX := /usr/local
BINDIR := $(PREFIX)/bin
LIBDIR := $(PREFIX)/lib
SHAREDIR := $(PREFIX)/share
INCLUDEDIR := $(PREFIX)/include

CC := cc
AR := ar
RANLIB := ranlib
CFLAGS := 
LDFLAGS := 

Q := @

all: minicas

minicas: main.o matrix.o operations.o variable.o 
	@echo '[01;32m  [LD]    [01;37mminicas[00m'
	$(Q)$(CC) -o minicas $(LDFLAGS) main.o matrix.o operations.o variable.o -lreadline

minicas.install: minicas
	@echo '[01;31m  [IN]    [01;37m$(BINDIR)/minicas[00m'
	$(Q)mkdir -p '$(DESTDIR)$(BINDIR)'
	$(Q)install -m0755 minicas $(DESTDIR)$(BINDIR)/minicas

minicas.clean:  main.o.clean matrix.o.clean operations.o.clean variable.o.clean
	@echo '[01;37m  [RM]    [01;37mminicas[00m'
	$(Q)rm -f minicas

minicas.uninstall:
	@echo '[01;37m  [RM]    [01;37m$(BINDIR)/minicas[00m'
	$(Q)rm -f '$(DESTDIR)$(BINDIR)/minicas'

main.o: main.c ./variable.h
	@echo '[01;34m  [CC]    [01;37mmain.o[00m'
	$(Q)$(CC) $(CFLAGS) -O0 -g -Wall -Wextra -c main.c  -O0 -g -Wall -Wextra -o main.o

main.o.install:

main.o.clean:
	@echo '[01;37m  [RM]    [01;37mmain.o[00m'
	$(Q)rm -f main.o

main.o.uninstall:

matrix.o: matrix.c ./matrix.h
	@echo '[01;34m  [CC]    [01;37mmatrix.o[00m'
	$(Q)$(CC) $(CFLAGS) -O0 -g -Wall -Wextra -c matrix.c  -O0 -g -Wall -Wextra -o matrix.o

matrix.o.install:

matrix.o.clean:
	@echo '[01;37m  [RM]    [01;37mmatrix.o[00m'
	$(Q)rm -f matrix.o

matrix.o.uninstall:

operations.o: operations.c ./operations.h
	@echo '[01;34m  [CC]    [01;37moperations.o[00m'
	$(Q)$(CC) $(CFLAGS) -O0 -g -Wall -Wextra -c operations.c  -O0 -g -Wall -Wextra -o operations.o

operations.o.install:

operations.o.clean:
	@echo '[01;37m  [RM]    [01;37moperations.o[00m'
	$(Q)rm -f operations.o

operations.o.uninstall:

variable.o: variable.c ./variable.h
	@echo '[01;34m  [CC]    [01;37mvariable.o[00m'
	$(Q)$(CC) $(CFLAGS) -O0 -g -Wall -Wextra -c variable.c  -O0 -g -Wall -Wextra -o variable.o

variable.o.install:

variable.o.clean:
	@echo '[01;37m  [RM]    [01;37mvariable.o[00m'
	$(Q)rm -f variable.o

variable.o.uninstall:

$(DESTDIR)$(PREFIX):
	@echo '[01;35m  [DIR]   [01;37m$(PREFIX)[00m'
	$(Q)mkdir -p $(DESTDIR)$(PREFIX)
$(DESTDIR)$(BINDIR):
	@echo '[01;35m  [DIR]   [01;37m$(BINDIR)[00m'
	$(Q)mkdir -p $(DESTDIR)$(BINDIR)
$(DESTDIR)$(LIBDIR):
	@echo '[01;35m  [DIR]   [01;37m$(LIBDIR)[00m'
	$(Q)mkdir -p $(DESTDIR)$(LIBDIR)
$(DESTDIR)$(SHAREDIR):
	@echo '[01;35m  [DIR]   [01;37m$(SHAREDIR)[00m'
	$(Q)mkdir -p $(DESTDIR)$(SHAREDIR)
$(DESTDIR)$(INCLUDEDIR):
	@echo '[01;35m  [DIR]   [01;37m$(INCLUDEDIR)[00m'
	$(Q)mkdir -p $(DESTDIR)$(INCLUDEDIR)
install: subdirs.install minicas.install main.o.install matrix.o.install operations.o.install variable.o.install
	@:

subdirs.install:

uninstall: subdirs.uninstall minicas.uninstall main.o.uninstall matrix.o.uninstall operations.o.uninstall variable.o.uninstall
	@:

subdirs.uninstall:

test: all subdirs subdirs.test
	@:

subdirs.test:

clean: minicas.clean main.o.clean matrix.o.clean operations.o.clean variable.o.clean

distclean: clean

dist: dist-gz dist-xz dist-bz2
	$(Q)rm -- $(PACKAGE)-$(VERSION)

distdir:
	$(Q)rm -rf -- $(PACKAGE)-$(VERSION)
	$(Q)ln -s -- . $(PACKAGE)-$(VERSION)

dist-gz: $(PACKAGE)-$(VERSION).tar.gz
$(PACKAGE)-$(VERSION).tar.gz: distdir
	@echo '[01;33m  [TAR]   [01;37m$(PACKAGE)-$(VERSION).tar.gz[00m'
	$(Q)tar czf $(PACKAGE)-$(VERSION).tar.gz \
		$(PACKAGE)-$(VERSION)/main.c \
		$(PACKAGE)-$(VERSION)/matrix.c \
		$(PACKAGE)-$(VERSION)/operations.c \
		$(PACKAGE)-$(VERSION)/variable.c \
		$(PACKAGE)-$(VERSION)/matrix.h \
		$(PACKAGE)-$(VERSION)/operations.h \
		$(PACKAGE)-$(VERSION)/variable.h

dist-xz: $(PACKAGE)-$(VERSION).tar.xz
$(PACKAGE)-$(VERSION).tar.xz: distdir
	@echo '[01;33m  [TAR]   [01;37m$(PACKAGE)-$(VERSION).tar.xz[00m'
	$(Q)tar cJf $(PACKAGE)-$(VERSION).tar.xz \
		$(PACKAGE)-$(VERSION)/main.c \
		$(PACKAGE)-$(VERSION)/matrix.c \
		$(PACKAGE)-$(VERSION)/operations.c \
		$(PACKAGE)-$(VERSION)/variable.c \
		$(PACKAGE)-$(VERSION)/matrix.h \
		$(PACKAGE)-$(VERSION)/operations.h \
		$(PACKAGE)-$(VERSION)/variable.h

dist-bz2: $(PACKAGE)-$(VERSION).tar.bz2
$(PACKAGE)-$(VERSION).tar.bz2: distdir
	@echo '[01;33m  [TAR]   [01;37m$(PACKAGE)-$(VERSION).tar.bz2[00m'
	$(Q)tar cjf $(PACKAGE)-$(VERSION).tar.bz2 \
		$(PACKAGE)-$(VERSION)/main.c \
		$(PACKAGE)-$(VERSION)/matrix.c \
		$(PACKAGE)-$(VERSION)/operations.c \
		$(PACKAGE)-$(VERSION)/variable.c \
		$(PACKAGE)-$(VERSION)/matrix.h \
		$(PACKAGE)-$(VERSION)/operations.h \
		$(PACKAGE)-$(VERSION)/variable.h

help:
	@echo '[01;37m :: minicas-0.42.0[00m'
	@echo ''
	@echo '[01;37mGeneric targets:[00m'
	@echo '[00m    - [01;32mhelp          [37mPrints this help message.[00m'
	@echo '[00m    - [01;32mall           [37mBuilds all targets.[00m'
	@echo '[00m    - [01;32mdist          [37mCreates tarballs of the files of the project.[00m'
	@echo '[00m    - [01;32minstall       [37mInstalls the project.[00m'
	@echo '[00m    - [01;32mclean         [37mRemoves compiled files.[00m'
	@echo '[00m    - [01;32muninstall     [37mDeinstalls the project.[00m'
	@echo ''
	@echo '[01;37mCLI-modifiable variables:[00m'
	@echo '    - [01;34mCC            [37m${CC}[00m'
	@echo '    - [01;34mCFLAGS        [37m${CFLAGS}[00m'
	@echo '    - [01;34mLDFLAGS       [37m${LDFLAGS}[00m'
	@echo '    - [01;34mDESTDIR       [37m${DESTDIR}[00m'
	@echo '    - [01;34mPREFIX        [37m${PREFIX}[00m'
	@echo '    - [01;34mBINDIR        [37m${BINDIR}[00m'
	@echo '    - [01;34mLIBDIR        [37m${LIBDIR}[00m'
	@echo '    - [01;34mSHAREDIR      [37m${SHAREDIR}[00m'
	@echo '    - [01;34mINCLUDEDIR    [37m${INCLUDEDIR}[00m'
	@echo ''
	@echo '[01;37mProject targets: [00m'
	@echo '    - [01;33mminicas       [37mbinary[00m'
	@echo ''
	@echo '[01;37mMakefile options:[00m'
	@echo '    - gnu:          true'
	@echo '    - colors:       true'
	@echo ''
	@echo '[01;37mRebuild the Makefile with:[00m'
	@echo '    zsh ./build.zsh -c -g'
.PHONY: all subdirs clean distclean dist install uninstall help

