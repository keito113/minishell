# CC      = cc
# CFLAGS  = -Wall -Wextra -Werror

# INCDIRS := . libft/includes
# CFLAGS  += $(addprefix -I,$(INCDIRS))

# LDLIBS  = -lreadline -lncurses
# LDFLAGS =

# NAME    = minishell

# SRCDIR  = src
# OBJDIR  = obj

# LIBFT_DIR = ./libft
# LIBFT_A   = $(LIBFT_DIR)/libft.a

# SRC := $(patsubst ./%,%,$(shell find $(SRCDIR) -type f -name '*.c'))

# OBJ := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))

# .PHONY: all clean fclean re print-%

# all: $(LIBFT_A) $(NAME)

# $(NAME): $(OBJ)
# 	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBFT_A) $(LDFLAGS) $(LDLIBS)

# $(LIBFT_A):
# 	$(MAKE) -C $(LIBFT_DIR)

# $(OBJDIR):
# 	mkdir -p $(OBJDIR)

# $(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
# 	@mkdir -p $(dir $@)
# 	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# clean:
# 	rm -rf $(OBJDIR)
# 	$(MAKE) -C $(LIBFT_DIR) clean

# fclean: clean
# 	rm -f $(NAME)
# 	$(MAKE) -C $(LIBFT_DIR) fclean

# re: fclean all


# =========================
#  Minishell AST demo build
# =========================
NAME        := minishell
CC          := cc
CFLAGS      := -Wall -Wextra -Werror -g

# --- libft (robust default & autodetect headers) ---
LIBFT_DIR   ?= ./libft
ifeq ($(strip $(LIBFT_DIR)),)
  LIBFT_DIR := ./libft
endif
LIBFT_A     := $(LIBFT_DIR)/libft.a

# libft.h の場所を自動検出（includes/ 配下 or 直下）
LIBFT_HDR_CAND := $(wildcard $(LIBFT_DIR)/includes/libft.h) $(wildcard $(LIBFT_DIR)/libft.h)
LIBFT_INC_DIR  := $(dir $(firstword $(LIBFT_HDR_CAND)))
ifeq ($(LIBFT_INC_DIR),)
  $(error libft.h not found under $(LIBFT_DIR). Set LIBFT_DIR=path/to/libft)
endif

# --- include path（正本を優先） ---
CPPFLAGS   := -Iinclude -I$(LIBFT_INC_DIR) -I.

# --- readline (mac / linux 自動判別) ---
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  READLINE_DIR ?= /opt/homebrew/opt/readline
  RLFLAGS := -I$(READLINE_DIR)/include -L$(READLINE_DIR)/lib -lreadline
else
  RLFLAGS := -lreadline -lncurses
endif

# --- sources / objects ---
SRC_DIR := src
OBJ_DIR := build

# src 以下の .c を全部拾う（既存の全ソースを対象）
SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# =========================
#  Rules
# =========================
all: $(NAME)

$(NAME): $(OBJS) $(LIBFT_A)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(RLFLAGS) $(LIBFT_A)

# libft を先に作る
$(LIBFT_A):
	$(MAKE) -C $(LIBFT_DIR)

# 汎用オブジェクト生成（依存関係も作成）
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MP -c $< -o $@

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -rf $(OBJ_DIR)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean || true
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
