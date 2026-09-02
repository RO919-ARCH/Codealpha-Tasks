# Makefile for CGPA Calculator (CLI & GUI)

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
MOC = moc

BIN_DIR = bin
BUILD_DIR = build
SRC_CLI_DIR = src/cli
SRC_GUI_DIR = src/gui

CLI_TARGET = $(BIN_DIR)/CGPA-Calculator
GUI_TARGET = $(BIN_DIR)/CGPA-Calculator-GUI

QT_CFLAGS = $(shell pkg-config --cflags Qt5Widgets Qt5Core Qt5Gui)
QT_LIBS = $(shell pkg-config --libs Qt5Widgets Qt5Core Qt5Gui)

.PHONY: all cli gui clean help

all: cli gui

cli: $(CLI_TARGET)

gui: $(GUI_TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CLI_TARGET): $(SRC_CLI_DIR)/CGPA-Calculator.cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BUILD_DIR)/main_gui.moc: $(SRC_GUI_DIR)/main_gui.cpp | $(BUILD_DIR)
	$(MOC) $< -o $@

$(GUI_TARGET): $(SRC_GUI_DIR)/main_gui.cpp $(BUILD_DIR)/main_gui.moc | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -fPIC -I$(BUILD_DIR) $(QT_CFLAGS) $< $(QT_LIBS) -o $@

clean:
	rm -rf $(BIN_DIR)/* $(BUILD_DIR)/*

help:
	@echo "Available targets:"
	@echo "  make all   - Build both CLI and GUI executables"
	@echo "  make cli   - Build CLI application ($(CLI_TARGET))"
	@echo "  make gui   - Build GUI application ($(GUI_TARGET))"
	@echo "  make clean - Remove compiled binaries and build artifacts"
