/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   onecommand.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asnaji <asnaji@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/27 10:15:51 by asnaji            #+#    #+#             */
/*   Updated: 2024/01/27 15:25:07 by asnaji           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char **join_args(t_tree *root)
{
	char **args;
	int args_size;
	t_cmd *temp;
	int i;

	args_size = 0;
	i = 0;
	temp = root->next;
	while(temp && temp->cmd[0] != '<' && temp->cmd[0] != '>')
	{	
		temp = temp->next;
		args_size++;
	}
	args = malloc((args_size + 1) * sizeof(char *));
	if(!args)
		return NULL;
	temp = root->next;	while(temp && temp->cmd[0] != '<' && temp->cmd[0] != '>')
	{	
		args[i] = ft_strdup(temp->cmd);
		i++;
		temp = temp->next;
	}
	args[args_size] = NULL;
	return (args);
}

void one_command_execution(t_tree *node, char **envp)
{
	char *absolutepath;
	char **args;
	int id;

	args = join_args(node);
	absolutepath = get_working_path(envp, node->data);
	id = fork();
	if(id == 0)
	{
		if(execve(absolutepath, args, envp) != 0)
			printf("invalid command\n");
		exit(EXIT_FAILURE);
	}
	wait(NULL);
	free(absolutepath);
	ft_free_array(args);
}
void findnodetoexecute(t_tree *root , char **env)
{
	if(root->tree_type == CMD)
		one_command_execution(root, env);
	if(root->left)
		findnodetoexecute(root->left, env);
	if(root->right)
		findnodetoexecute(root->right, env);
}
