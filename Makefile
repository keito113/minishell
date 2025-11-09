
# Makefile (root)
NAME        := minishell

CC          := cc
CFLAGS      := -Wall -Wextra -Werror -MMD -MP
# デバッグ時は: make SAN=1 で ASan+g3 を付与
ifeq ($(SAN),1)
CFLAGS      += -fsanitize=address -g3
LDFLAGS     += -fsanitize=address
endif

# Dirs
SRC_DIR     := src
INC_DIR     := include
LIBFT_DIR   := libft
BUILD_DIR   := .build

# libft
LIBFT_A     := $(LIBFT_DIR)/libft.a
INCLUDES    := -I$(INC_DIR) -I$(LIBFT_DIR)/includes

# readline (mac / linux 自動判別)
UNAME_S     := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
BREW_PREFIX ?= $(shell brew --prefix 2>/dev/null)
RL_INC      := -I$(BREW_PREFIX)/opt/readline/include
RL_LIB      := -L$(BREW_PREFIX)/opt/readline/lib -lreadline
else
RL_INC      :=
# 環境により -lncurses か -ltinfo が必要
RL_LIB      := -lreadline -lncurses
endif

# Sources / Objects
SRCS        := $(shell find $(SRC_DIR) -name "*.c")
OBJS        := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS        := $(OBJS:.o=.d)

# ===================================================

.PHONY: all clean fclean re run show count

all: $(NAME)

$(NAME): $(LIBFT_A) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFT_DIR) -lft $(RL_LIB) $(LDFLAGS) -o $@

# libft を先にビルド
$(LIBFT_A):
	$(MAKE) -C $(LIBFT_DIR)

# 各 .o を .build/ 配下に生成（ディレクトリ自動作成）
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

show:
	@echo "SRCS: $(words $(SRCS)) files"
	@echo "OBJS: $(words $(OBJS)) files"

count:
	@find $(SRC_DIR) -name "*.c" -print0 | xargs -0 wc -l | sort -n

-include $(DEPS)
