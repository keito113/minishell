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

SRCS := \
    src/built_in/cd_builtin/built_absolute_path.c \
    src/built_in/cd_builtin/cd_builtin.c \
    src/built_in/cd_builtin/normalize_absolute_path.c \
    src/built_in/cd_builtin/path_vec_to_absolute.c \
    src/built_in/cd_builtin/path_vec_utils.c \
    src/built_in/echo_builtin/echo_builtin.c \
    src/built_in/env_builtin/env_builtin.c \
    src/built_in/exec_builtin.c \
    src/built_in/exit_builtin/exit_builtin.c \
    src/built_in/exit_builtin/exit_utils.c \
    src/built_in/export_builtin/env_append.c \
    src/built_in/export_builtin/export_builtin.c \
    src/built_in/export_builtin/handle_export_arg.c \
    src/built_in/export_builtin/print_export_sorted.c \
    src/built_in/is_buiitin_name.c \
    src/built_in/pwd_builtin/pwd_builtin.c \
    src/built_in/restore_builtin_redirects.c \
    src/built_in/setup_builtin_redirects.c \
    src/built_in/unset_builtin/unset_builtin.c \
    src/env_table/env_utils.c \
    src/env_table/make_envtable.c \
    src/error/exec_error.c \
    src/error/status.c \
    src/exec/apply_redirs.c \
    src/exec/exec.c \
    src/exec/exec_child_single.c \
    src/exec/exec_external.c \
    src/exec/find_cmd_path.c \
    src/exec/open_src_fd.c \
    src/exec/run_single_command.c \
    src/expand/expand.c \
    src/expand/expand_chunk.c \
    src/expand/expand_cmd.c \
    src/expand/expand_cmd_utils.c \
    src/expand/expand_utils.c \
    src/expand/expand_word.c \
    src/expand/insert_fields.c \
    src/expand/split_fields.c \
    src/heredoc/close_hdocs.c \
    src/heredoc/hdoc_sig.c \
    src/heredoc/hdocs_sig_utils.c \
    src/heredoc/heredoc.c \
    src/input/read_line.c \
    src/main.c \
    src/parse/bad_fd.c \
    src/parse/dup_word_info.c \
    src/parse/free_ast.c \
    src/parse/free_cmd.c \
    src/parse/parse.c \
    src/parse/parse_cmd.c \
    src/parse/precheck_syntax.c \
    src/parse/push_redir.c \
    src/parse/push_word_info.c \
    src/parse/push_word_to_argv.c \
    src/parse/syntax_error.c \
    src/pipe/build_pipeline_cmds.c \
    src/pipe/pipe_ctx.c \
    src/pipe/pipe_exec.c \
    src/pipe/pipe_heredoc.c \
    src/pipe/pipeline_child.c \
    src/pipe/pipeline_error.c \
    src/pipe/pipe_status.c \
    src/pipe/run_pipeline.c \
    src/run_prompt_loop.c \
    src/signals/sig_setup.c \
    src/tokens/lexer.c \
    src/tokens/tokens_lex.c \
    src/tokens/tokens_lex_helpers.c \
    src/tokens/tokens_lex_run.c \
    src/tokens/tokens_post.c \
    src/tokens/tokens_utils.c \
    src/tokens/tokens_utils_fd.c \
    src/tokens/tokens_vec.c \
    src/tokens/tokens_vec_free.c \
    src/tokens/tokens_word.c \
    src/utils.c

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

