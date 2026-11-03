PKG_CONFIG ?= pkg-config
CC ?= cc

PKGS      := gtk4
CFLAGS    += -std=c11 -Wall -Wextra -O2 $(shell $(PKG_CONFIG) --cflags $(PKGS))
LDLIBS    += $(shell $(PKG_CONFIG) --libs $(PKGS))

BUILD_DIR  := build
SRC        := src/main.c src/window.c
RESOURCE_C := $(BUILD_DIR)/resources.c
RESOURCE_XML := data/arkium.gresource.xml
RESOURCE_DEPS := data/style.css
TARGET     := $(BUILD_DIR)/arkium

.PHONY: all run clean install

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(RESOURCE_C): $(RESOURCE_XML) $(RESOURCE_DEPS) | $(BUILD_DIR)
	glib-compile-resources --sourcedir=data --target=$@ --generate-source $(RESOURCE_XML)

$(TARGET): $(SRC) $(RESOURCE_C) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Isrc -I$(BUILD_DIR) -o $@ $(SRC) $(RESOURCE_C) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

# Adjust PREFIX for a user-local or system install.
PREFIX ?= /usr/local

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/arkium
	install -Dm644 data/io.github.raeark.Arkium.desktop \
		$(DESTDIR)$(PREFIX)/share/applications/io.github.raeark.Arkium.desktop
	install -Dm644 data/icons/io.github.raeark.Arkium.svg \
		$(DESTDIR)$(PREFIX)/share/icons/hicolor/scalable/apps/io.github.raeark.Arkium.svg
