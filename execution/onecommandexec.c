/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   onecommandexec.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asnaji <asnaji@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/15 17:07:34 by asnaji            #+#    #+#             */
/*   Updated: 2024/02/16 21:54:01 by asnaji           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
// #include <cstddef>

void	execute_cmd(char *abpath, char **envp, char **args)
{
	if (execve(abpath, args, envp) != 0)
	{
		wrerror("turboshell: command not found: ");
		wrerror(args[0]);
		wrerror("\n");
	}
	exit(127);
}

int	exec_cmd1(int infile, int outfile, char **args, t_env *env)
{
	pid_t	id;
	int		status;
	char	*absolutepath;
	char	**envp;

	envp = env_to_arr(env);
	if (access(args[0], X_OK) != 0)
		absolutepath = get_working_path(envp, args[0]);
	else
		absolutepath = ft_strdup(args[0]);
	id = fork();
	if (id == -1)
	{
		wrerror("turboshell: fork: Resource temporarily unavailable\n");
		return (127);
	}
	if (id == 0)
	{
		changeinfile(infile);
		changeoutfile(outfile);
		execute_cmd(absolutepath, envp, args);
	}
	wait(&status);
	exitstatus(WEXITSTATUS(status), 1);
	return (status);
}

void make_args_node(t_cmd **args, char *buffer, int spaceafter, int *flag, int expand, int heredocfd, int heredocexpand)
{
	t_cmd *new;
	t_cmd *curr;

	new = malloc(sizeof(t_cmd));
	if(!new)
		return;
	new->spaceafter = spaceafter;
	new->cmd = buffer;
	new->expandwildcard = expand;
	new->expandheredoc = heredocexpand;
	new->next = NULL;
	new->heredocfd = heredocfd;
	if(*flag == 1)
	{
		*args = new;
		*flag = 0;
	}
	else {
		curr = *args;
		while(curr->next)
			curr = curr->next;
		curr->next = new;
	}
}

t_cmd *make_args_lst(t_cmd *cmd, t_env *env)
{
	t_cmd	*new;
	int		flag;
	char	*buffer;
	char 	**tmp;
	int		j;

	flag = 1;
	buffer = NULL;
	while(cmd)
	{
		if(cmd->cmd && cmd->cmd[0] == '$') 
		{
			tmp = var_toarray(cmd->cmd, env);
			if (tmp)
			{
				j = 0;
				while (tmp[j])
				{
					buffer = ft_strdup(tmp[j]);
					if(j == 0)
					{
						if(check_expanded_var(cmd->cmd, env)== 1 || cmd->spaceafter == 1)
							make_args_node(&new, buffer, 1, &flag , 1, cmd->heredocfd, cmd->expandheredoc);
						else
							make_args_node(&new, buffer, 0, &flag , 1, cmd->heredocfd, cmd->expandheredoc);
						buffer = NULL;
					}
					else{
						make_args_node(&new, buffer, 1, &flag , 1, cmd->heredocfd, cmd->expandheredoc);
						buffer = NULL;
					}
					j++;
				}
			}
			else if(cmd->cmd && cmd->cmd[0] == '$' && !cmd->cmd[1]) 
			{
				buffer = argextraction(cmd, env);
				if(cmd->cmd && (cmd->cmd[0] == '\'' || cmd->cmd[0] == '\"'))
					make_args_node(&new, buffer, cmd->spaceafter, &flag, 0, cmd->heredocfd, cmd->expandheredoc);
				else
					make_args_node(&new, buffer, cmd->spaceafter, &flag, 1, cmd->heredocfd, cmd->expandheredoc);
			}
			else
			{
				buffer = argextraction(cmd, env);
				if(cmd->cmd && (cmd->cmd[0] == '\'' || cmd->cmd[0] == '\"'))
					make_args_node(&new, buffer, cmd->spaceafter, &flag, 0, cmd->heredocfd, cmd->expandheredoc);
				else
					make_args_node(&new, buffer, cmd->spaceafter, &flag, 1, cmd->heredocfd, cmd->expandheredoc);
			}
		}
		else if(cmd->cmd)
		{
			buffer = argextraction(cmd, env);
			if(cmd->cmd[0] == '\'' || cmd->cmd[0] == '\"')
				make_args_node(&new, buffer, cmd->spaceafter, &flag, 0, cmd->heredocfd, cmd->expandheredoc);
			else
				make_args_node(&new, buffer, cmd->spaceafter, &flag, 1, cmd->heredocfd, cmd->expandheredoc);
		}
		cmd = cmd->next;
	}
	return new;
}

t_cmd *joined_args(t_cmd *args)
{
	t_cmd *new;
	int		flag;
	int		expand;
	char	*buffer;
	int		spaceafter;
	
	flag = 1;
	expand = 0;
	buffer = NULL;
	while(args)
	{
		skip_redirections(&args);
		expand = 0;
		if(args)
			spaceafter = args->spaceafter;
		if(args && args->cmd && isredirection(args->cmd[0]) == 0)
		{
			args->spaceafter = 0;
			while(args && args->spaceafter != 1)
			{
				if(args->cmd && isredirection(args->cmd[0]) == 1)
					break;
				if(args->cmd && ft_strchr(args->cmd, '*'))
					expand = args->expandwildcard;
				if(args->cmd)
					buffer = ft_strjoin(buffer, args->cmd);
				args = args->next;
			}
		}
		else if(args && args->cmd)
		{
			if(args->cmd && ft_strchr(args->cmd, '*'))
					expand = args->expandwildcard;
			if(args->cmd)
				buffer = ft_strjoin(buffer, args->cmd);
			args = args->next;
		}
		else if (args){
			args = args->next;
		}
		make_args_node(&new, buffer, spaceafter, &flag, expand, 0, 0);
		buffer = NULL;
	}
	return new;
}

int	one_command_execution(t_tree *node, t_env *env)
{
	char	**args;
	t_cmd	*new;
	int		infile;
	int		outfile;
	t_cmd	*lst_args;
	t_cmd	*new_joinedargs;

	infile = 0;
	outfile = 1;
	new = new_cmd_list(node->next, env);
	lst_args = make_args_lst(new, env);
	new_joinedargs = joined_args(lst_args);
	t_cmd *temp;
	temp = lst_args;
	while(temp)
	{
		printf("{%s} [%d] [%d] [%d]\n", temp->cmd, temp->spaceafter, temp->expandheredoc, temp->heredocfd);
		temp = temp->next;
	}
	// return (0);
	
	args = get_all_wildcards(new_joinedargs);
	infile = getlastinfile(lst_args, env);
	outfile = getlastoutfile(lst_args);
	if (outfile == -1 || infile == -1)
		return (1);
	// args = join_args1(get_command_start(new), env);
	if (!args || !args[0])
		return (0);
	if (is_builtin(args[0]))
		return (builtinexec(args, &env, infile, outfile));
	return (exec_cmd1(infile, outfile, args, env));
}
