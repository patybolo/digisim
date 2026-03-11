
.PHONY: help all linux windows clean install uninstall

CC      ?= gcc
CFLAGS  := -O3 -Wall -Wextra -Iheader -Ilibs/raylib/src

# Determine the host OS
HOST_OS := $(shell uname -s 2>/dev/null || echo Windows_NT)

ifeq ($(HOST_OS),Windows_NT)
    NPROC := $(NUMBER_OF_PROCESSORS)
else
    NPROC := $(shell nproc)
endif

# Default target OS is the host
TARGET_OS ?= $(HOST_OS)

ifeq ($(TARGET_OS),Windows)
    CC      := x86_64-w64-mingw32-gcc
    AR      := x86_64-w64-mingw32-ar
    LDFLAGS := -static -lopengl32 -lgdi32 -lwinmm
    TARGET  := build/digisim.exe
    RAYLIB_MAKE_FLAGS := PLATFORM=PLATFORM_DESKTOP OS=Windows_NT CC=$(CC) AR=$(AR) -j$(NPROC)
else
    LDFLAGS := -lm -lpthread -ldl -lrt -lX11
    TARGET  := build/digisim
    RAYLIB_MAKE_FLAGS := PLATFORM=PLATFORM_DESKTOP -j$(NPROC)
endif

SRCFILES := $(wildcard src/*.c)
OBJFILES := $(patsubst src/%.c,build/%.o,$(SRCFILES))

RAYLIB_DIR := libs/raylib/src
RAYLIB_LIB := $(RAYLIB_DIR)/libraylib.a

# OS switch detection and auto-clean
LAST_OS := $(shell cat build/.build_os 2>/dev/null)
ifneq ($(LAST_OS),)
ifneq ($(LAST_OS),$(TARGET_OS))
$(info [*] Target OS changed from $(LAST_OS) to $(TARGET_OS). Cleaning workspace...)
_ := $(shell $(MAKE) clean > /dev/null 2>&1)
endif
endif

help:
	@echo "[!] Usage: make (target)"
	@echo "    make all       =>  Compiles the whole project (Host OS)"
	@echo "    make windows   =>  Cross-compiles for Windows from Linux (requires mingw-w64)"
	@echo "    make clean     =>  Clean up the compiled binaries"
	@echo "    make install   =>  Installs digisim to /usr/local/bin/ (Linux builds only)"
	@echo "    make uninstall =>  Removes digisim from /usr/local/bin/ (Linux builds only)"

all: $(TARGET)

linux:
	@$(MAKE) all TARGET_OS=Linux

windows:
	@$(MAKE) all TARGET_OS=Windows

$(RAYLIB_LIB):
	@echo "[*] Building raylib..."
	@$(MAKE) -C $(RAYLIB_DIR) $(RAYLIB_MAKE_FLAGS) > /dev/null 2>&1
	@echo "[*] raylib built successfully"

$(TARGET): $(OBJFILES) $(RAYLIB_LIB)
	@$(CC) $(OBJFILES) $(RAYLIB_LIB) -o $@ $(LDFLAGS)
	@echo "$(TARGET_OS)" > build/.build_os
	@echo "[*] LD $@"

build/%.o: src/%.c | build
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "[*] CC $< -> $@"

build:
	@mkdir -p build

clean:
	@echo "[*] Cleaning up..."
	@rm -rf build
	@$(MAKE) -C $(RAYLIB_DIR) clean > /dev/null 2>&1
	@echo "[*] raylib cleaned"

install: $(TARGET)
	@if [ "$(TARGET_OS)" = "Windows" ]; then \
		echo "[!] Cannot install Windows executable natively using make install. Use a Linux build."; \
	else \
		install -Dm755 $(TARGET) /usr/local/bin/digisim; \
		echo "[*] Installed digisim to /usr/local/bin/"; \
	fi

uninstall:
	@rm -f /usr/local/bin/digisim
	@echo "[*] Uninstalled digisim from /usr/local/bin/"
