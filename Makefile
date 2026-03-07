
.PHONY: help all clean install uninstall

CC      := gcc
CFLAGS  := -Wall -Wextra -Iheader -Ilibs/raylib/src
LDFLAGS := -lm -lpthread -ldl -lrt -lX11

SRCFILES := $(wildcard src/*.c)
OBJFILES := $(patsubst src/%.c,build/%.o,$(SRCFILES))
TARGET   := build/digisim

RAYLIB_DIR := libs/raylib/src
RAYLIB_LIB := $(RAYLIB_DIR)/libraylib.a

help:
	@echo "[!] Usage: make (target)"
	@echo "    make all       =>  Compiles the whole project"
	@echo "    make clean     =>  Clean up the compiled binaries"
	@echo "    make install   =>  Installs digisim to /usr/bin/"
	@echo "    make uninstall =>  Removes digisim from /usr/bin/"

all: $(TARGET)

$(RAYLIB_LIB):
	@echo "[*] Building raylib..."
	@$(MAKE) -C $(RAYLIB_DIR) PLATFORM=PLATFORM_DESKTOP -j$(nproc) > /dev/null 2>&1
	@echo "[*] raylib built successfully"

$(TARGET): $(OBJFILES) $(RAYLIB_LIB)
	@$(CC) $(OBJFILES) $(RAYLIB_LIB) -o $@ $(LDFLAGS)
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
	@install -Dm755 $(TARGET) /usr/bin/digisim
	@echo "[*] Installed digisim to /usr/bin/"

uninstall:
	@rm -f /usr/bin/digisim
	@echo "[*] Uninstalled digisim from /usr/bin/"
