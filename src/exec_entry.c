#include "minishell.h"

int	exec_entry(t_ast *root, t_shell *sh)
{
	if (!root)
		return (0);
	if (root->type == AST_CMD)
		run_single_command(&root->as.cmd, sh);
	// else if(root->type == AST_PIPE)
	// 	flatten_pipeline();
	// 	run_pipeline();
	return (0);
}
