CC := g++
SRC_DIR := src
INC_DIR := inc
BIN_DIR := bin

SRCS := main.c $(SRC_DIR)/socket.c

OBJS := $(SRCS:%.c=%.o)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -pthread -c $< -o $@ -I$(INC_DIR)


all: $(OBJS)
	$(CC) -pthread -o main $(OBJS)

clean:
	rm -rf main $(OBJS)
