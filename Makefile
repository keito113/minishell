CC      = cc
CFLAGS  = -Wall -Wextra -Werror

INCDIRS := . libft/includes
CFLAGS  += $(addprefix -I,$(INCDIRS))

LDLIBS  = -lreadline -lncurses
LDFLAGS =

NAME    = minishell

SRCDIR  = src
OBJDIR  = obj

LIBFT_DIR = ./libft
LIBFT_A   = $(LIBFT_DIR)/libft.a

SRC := $(patsubst ./%,%,$(shell find $(SRCDIR) -type f -name '*.c'))

OBJ := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))

.PHONY: all clean fclean re print-%

all: $(LIBFT_A) $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBFT_A) $(LDFLAGS) $(LDLIBS)

$(LIBFT_A):
	$(MAKE) -C $(LIBFT_DIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJDIR)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all


