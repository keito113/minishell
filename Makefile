NAME        := minishell

CC          := cc
CFLAGS      := -Wall -Wextra -Werror

SRC_DIR     := src
INC_DIR     := include
LIBFT_DIR   := libft
BUILD_DIR   := .build

LIBFT_A     := $(LIBFT_DIR)/libft.a
INCLUDES    := -I$(INC_DIR) -I$(LIBFT_DIR)/includes

RL_INC      :=
RL_LIB      := -lreadline -lncurses

SRCS        := $(shell find $(SRC_DIR) -name "*.c")
OBJS        := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean fclean re run show count

all: $(NAME)

$(NAME): $(LIBFT_A) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFT_DIR) -lft $(RL_LIB) $(LDFLAGS) -o $@

$(LIBFT_A):
	$(MAKE) -C $(LIBFT_DIR)

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $(RL_INC) -c $< -o $@

run: $(NAME)
	./$(NAME)

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -rf $(BUILD_DIR)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

