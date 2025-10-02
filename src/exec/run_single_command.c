#include "minishell.h"

int	run_single_command(t_cmd *cmd, t_shell *sh)
{
	pid_t	pid;
	int		status;

	pid = 0;
	if (!cmd->argv || !cmd->argv[0])
	{
		sh->last_status = 1;
		return (sh->last_status);
	}
	if (prepare_cmd_heredocs(cmd, sh, NULL) != 0)
	{
		// 途中で失敗/中断（130など）したら実行しない
		close_hdocs_in_cmd(cmd);
		return (sh->last_status);
	}
	if (cmd->is_builtin)
	{
		//あとでビルトイン実装
		sh->last_status = 1;
		return (sh->last_status);
	}
	pid = fork();
	if (pid < 0)
	{
		sh->last_status = 1;
		close_hdocs_in_cmd(cmd);
		return (1);
	}
	if (pid == 0)
	{
		if (apply_redirs(cmd) < 0)
			exit(1);
		exec_external(cmd->argv, sh->envp);
	}
	waitpid(pid, status, 0);
	if (WIFEXITED(status))
		sh->last_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		sh->last_status = 128 + WTERMSIG(status);
	else
		sh->last_status = 1;
	close_hdocs_in_cmd(cmd);
	return (0);
}

static int	inter_target_fd(const t_redir *redir)
{
	if (!redir)
		return (-1);
	if (redir->fd_target >= 0)
		return (redir->fd_target);
	if (redir->kind == R_IN || redir->kind == R_HDOC)
		return (STDIN_FILENO);
	return (STDOUT_FILENO);
}

static int	open_src_fd(t_redir *redir)
{
	int	src_fd;

	src_fd = -1;
	if (redir->kind == R_HDOC)
	{
		src_fd = redir->hdoc_fd;
		if (src_fd < 0)
			return (-1);
	}
	else if (redir->kind == R_IN)
	{
		src_fd = open(redir->arg, O_RDONLY);
		if (src_fd < 0)
			return (perror(redir->arg), -1);
	}
	else if (redir->kind == R_OUT)
	{
		src_fd = open(redir->arg, O_CREAT | O_WRONLY | O_TRUNC, 0644);
		if (src_fd < 0)
			return (perror(redir->arg), -1);
	}
	else if (redir->kind == R_APP)
	{
		src_fd = open(redir->arg, O_CREAT | O_WRONLY | O_APPEND, 0644);
		if (src_fd < 0)
			return (perror(redir->arg), -1);
	}
	return (src_fd);
}

int	apply_redirs(const t_cmd *cmd)
{
	t_redir	*redir;
	int		from_fd;
	int		to_fd;

	if (!cmd)
		return (-1);
	redir = cmd->redirs;
	while (redir)
	{
		from_fd = -1;
		to_fd = inter_target_fd(redir);
		if (to_fd < 0 || to_fd >= 1024) //最適なfd数に変更
			return (perror("bad target fd"), -1);
		from_fd = open_src_fd(redir);
		if (from_fd < 0)
			return (-1);
		if (from_fd != to_fd)
		{
			if (dup2(from_fd, to_fd) < 0)
			{
				perror("dup2");
				close(from_fd);
				return (-1);
			}
		}
		if (redir->kind != R_HDOC || (redir->kind == R_HDOC
				&& from_fd != to_fd))
			close(from_fd);
		redir = redir->next;
	}
	return (0);
}
