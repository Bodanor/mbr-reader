NAME        := mbr-reader

CC=gcc
CFLAGS=-Wall -Wextra -MMD -MP -g

C_SOURCE := $(wildcard *.c)
OBJS        := $(C_SOURCE:%.c=%.o)
DEPS        := $(OBJS:.o=.d)

RM          := rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(info CC : $@)
	$(CC) $(CFLAGS) $(OBJS) -o $@

-include $(DEPS)
%.o: %.c
	$(info CC : $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(DEPS)
	$(RM) $(NAME)

.PHONY: clean