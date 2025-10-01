# Compiler and flags
CC = gcc
CFLAGS = -g

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Files
TARGET = main
SRC = $(filter-out $(SRC_DIR)/test.c $(SRC_DIR)/endian_reverser.c, $(wildcard $(SRC_DIR)/*.c))
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Default rule
all: $(TARGET)

# Linking
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# Compiling
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure obj directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean
