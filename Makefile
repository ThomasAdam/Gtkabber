# Makefile.  Mostly stolen from Vimprobable.

TARGET = gtkabber

# Objectfiles, needed for $(TARGET)
OBJ = gtkabber.o callback.o
# Manpages
#MAN = gtkabber.1
# Used libraries to get needed CFLAGS and LDFLAGS form pkg-config
LIBS = gtk+-2.0 unique-1.0
# Files to removo by clean target
CLEAN = $(TARGET) $(OBJ) $(DEPS)
# Files to install by install target or remove by uninstall target
INSTALL = $(BINDIR)/$(TARGET) 
###$(addprefix $(MANDIR)/man1/,$(MAN))

# DEBUG build?  Off by default
V_DEBUG = 0

CFLAGS += `pkg-config --cflags $(LIBS)`
LDFLAGS += `pkg-config --libs $(LIBS)` -lX11 -lXext

# TA:  This is a pretty stringent list of warnings to bail on!
ifeq ($(V_DEBUG),1)
CFLAGS += -g -ggdb -ansi -Wstrict-prototypes
CFLAGS += -Wno-long-long -Wall -Wmissing-declarations
endif

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
##MANDIR ?= $(PREFIX)/share/man
# Mode bits for normal not executable files
FMOD ?= 0644
# Mode bits for directories
DMOD ?= 0755
# Mode bits for executables
EXECMOD ?= 0755
# Destination directory to install files
DESTDIR ?= /

# auto garerated dependancies for object files
DEPS = $(OBJ:%.o=%.d)

all: $(TARGET)

-include $(DEPS)

$(TARGET): $(OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

.PHONY: clean install uninstall
clean:
	-rm -f $(CLEAN)
install: $(addprefix $(DESTDIR)/,$(INSTALL))
uninstall:
	rm -f $(INSTALL)

# pattern rule to inslall executabels
$(DESTDIR)/$(BINDIR)/%: ./%
	-[ -e '$(@D)' ] || mkdir -p '$(@D)' && chmod $(DMOD) '$(@D)'
	cp -f '$<' '$@'
	-strip -s '$@'
	chmod $(EXECMOD) '$@'

# pattern rule to install manpages
#$(DESTDIR)/$(MANDIR)/man1/%: ./%
#	-[ -e '$(@D)' ] || mkdir -p '$(@D)' && chmod $(DMOD) '$(@D)'
#	cp -f '$<' '$@'
#	chmod $(FMOD) '$@'

%.o: %.c
	$(CC) -MMD -c $(CFLAGS) $< -o $@ 
