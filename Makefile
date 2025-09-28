CC = cc
CFLAGS = -Wall -Wextra -Werror

INCDIRS := . libft/includes
CFLAGS  += $(addprefix -I,$(INCDIRS))

LDLIBS = -lreadline -lncurses

NAME = minishell


SRCDIR = src
OBJDIR = obj

LIBFT_DIR = ./libft
LIBFT_A = $(LIBFT_DIR)/libft.a


SRC = $(wildcard $(SRCDIR)/*.c)


OBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))
DEP = $(OBJ:.o=.d)


all: $(LIBFT_A) $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) $(LIBFT_A) $(LDFLAGS) $(LDLIBS)

$(LIBFT_A):
	make -C $(LIBFT_DIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJ)
	make -C $(LIBFT_DIR) clean

fclean: clean
	rm -f $(NAME)
	make -C $(LIBFT_DIR) fclean

re: fclean all

-include $(DEP)

.PHONY: all clean fclean re