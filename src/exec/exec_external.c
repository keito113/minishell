#include "minishell.h"

static void	validate_and_exec(const char *path, char *const argv[],
		char *const envp[])
{
	// 実体の存在チェック（ここで ENOENT を弾く）
	if (access(path, F_OK) != 0)
		exit(127);

	// ディレクトリは不可


	// 実行権限
	if (access(path, X_OK) != 0)
		exit(126);
	execve(path, argv, envp);
	//エラー情報の保持の
	// ここに来たら execve 失敗
	if (errno == ENOENT)
		exit(127);
	else
		exit(126);
}

void	exec_external(char *const argv[], char *const envp[])
{
	if (!argv || !argv[0])
		exit(0);

	char *path = find_cmd_path(argv[0], envp);
	if (!path)
		exit(127);

	validate_and_exec(path, argv, envp);
}