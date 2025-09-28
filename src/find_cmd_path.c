
#include "minishell.h"

static char	*try_paths(char **paths, const char *cmd)
{
	char		*full_path;
	char		*temp;
	int			i;
	const char	*base;

	i = 0;
	while (paths[i])
	{
		if (*paths[i] == '\0')
			base = ".";
		else
			base = paths[i];
		temp = ft_strjoin(base, "/");
		full_path = ft_strjoin(temp, cmd);
		free(temp);
		if (access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
		i++;
	}
	return (NULL);
}

static void	free_split(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

char	*find_cmd_path(const char *cmd, char *const *envp)
{
	int		i;
	char	*path_env;
	char	**paths;
	char	*result;

	i = 0;
	if (!cmd || *cmd == '\0')
		return (NULL);
	if (access(cmd, X_OK) == 0)
		return (ft_strdup(cmd));
	while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
		i++;
	if (!envp[i])
		return (NULL);
	path_env = envp[i] + 5;
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	result = try_paths(paths, cmd);
	free_split(paths);
	return (result);
}
