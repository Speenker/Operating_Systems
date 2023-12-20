CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -ldl

SRC_DIR = .
BUILD_DIR = build

SRC_FILES = dynamic.c first_lib.c second_lib.c
OBJ_FILES = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC_FILES))
LIB_FILES = $(patsubst %.c, $(BUILD_DIR)/lib%.so, $(filter-out dynamic.c, $(SRC_FILES)))

.PHONY: all clean

all: dynamic static_first static_second

dynamic: $(OBJ_FILES) $(LIB_FILES)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lib%.so: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -fPIC -shared $< -o $@

static_first: $(BUILD_DIR)/static_first.o $(BUILD_DIR)/first_lib.a
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/static_first.o: static.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/first_lib.a: $(BUILD_DIR)/first_lib.o
	ar rcs $@ $<

$(BUILD_DIR)/first_lib.o: first_lib.c
	$(CC) $(CFLAGS) -c $< -o $@

static_second: $(BUILD_DIR)/static_second.o $(BUILD_DIR)/second_lib.a
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/static_second.o: static.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/second_lib.a: $(BUILD_DIR)/second_lib.o
	ar rcs $@ $<

$(BUILD_DIR)/second_lib.o: second_lib.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) dynamic static_first static_second