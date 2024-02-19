/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   recursive_parsing.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaazaa <yzaazaa@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 12:05:06 by asnaji            #+#    #+#             */
/*   Updated: 2024/02/19 19:13:10 by yzaazaa          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int exitstatus(int newstatus, int flag)
{
	static int status;

	if(flag == 1)
		status = newstatus;
	return status;
}

int hasspaceafter(char *str, int i)
{
	if(i == -1)
		return 0;
	if(ft_isspace(str[i]) == 1)
		return 1;
	return 0;
}

int	ft_char(t_token **cmd, t_vars *vars)
{
	vars->tmp = vars->i;
	while (vars->cmd[vars->i] && islimiter(vars->cmd[vars->i]) == 0
		&& ft_isquote(vars->cmd[vars->i]) == 0 && ft_isspace(vars->cmd[vars->i]) == 0
		&& (vars->cmd[vars->i] != ')' && vars->cmd[vars->i] != '('))
		vars->i++;
	if (vars->flag == 1)
	{
		(*cmd)->cmd = ft_substr(vars->cmd, vars->tmp, vars->i - vars->tmp);
		(*cmd)->spaceafter = hasspaceafter(vars->cmd, vars->tmp - 1);
		vars->flag = 0;
	}
	else
		ft_newnode(cmd, ft_substr(vars->cmd, vars->tmp, vars->i - vars->tmp), hasspaceafter(vars->cmd, vars->tmp - 1));
	return (ft_switch(cmd, vars));
}

int	ft_separator(t_token **cmd, t_vars *vars)
{
	int		x;
	char	save;

	vars->tmp = vars->i;
	x = 0;
	save = vars->cmd[vars->i];
	while (vars->cmd[vars->i] && islimiter(vars->cmd[vars->i]) == 1
		&& ft_isquote(vars->cmd[vars->i]) == 0
		&& ft_isspace(vars->cmd[vars->i]) == 0 && x++ < 2 && save == vars->cmd[vars->i])
		vars->i++;
	if (vars->flag == 1)
	{
		(*cmd)->cmd = ft_substr(vars->cmd, vars->tmp, vars->i - vars->tmp);
		(*cmd)->spaceafter = hasspaceafter(vars->cmd, vars->tmp - 1);
		vars->flag = 0;
	}
	else
		ft_newnode(cmd, ft_substr(vars->cmd, vars->tmp, vars->i - vars->tmp), hasspaceafter(vars->cmd, vars->tmp - 1));
	return (ft_switch(cmd, vars));
}

int	ft_space(t_token **cmd, t_vars *vars)
{
	vars->tmp = vars->i;
	while (vars->cmd[vars->i] && islimiter(vars->cmd[vars->i]) == 0
		&& ft_isquote(vars->cmd[vars->i]) == 0 && ft_isspace(vars->cmd[vars->i]) == 1)
		vars->i++;
	return (ft_switch(cmd, vars));
}

int	ft_bracket(t_token **cmd, t_vars *vars)
{
	vars->tmp = vars->i;
	if (vars->cmd[vars->i] == '(' || vars->cmd[vars->i] == ')')
		vars->i++;
	if (vars->flag == 1)
	{
		(*cmd)->cmd = ft_substr(vars->cmd, vars->tmp, vars->i - vars->tmp);
		(*cmd)->spaceafter = hasspaceafter(vars->cmd, vars->tmp - 1);
		vars->flag = 0;
	}
	else
		ft_newnode(cmd, ft_substr(vars->cmd, vars->tmp, vars->i - vars->tmp),  hasspaceafter(vars->cmd, vars->tmp - 1));
	return (ft_switch(cmd, vars));
}

int	ft_quote(t_token **cmd, t_vars *vars)
{
	char	save;

	vars->tmp = vars->i;
	if (vars->cmd[vars->i] == '\'' || vars->cmd[vars->i] == '"')
		save = vars->cmd[vars->i++];
	while (vars->cmd[vars->i] && vars->cmd[vars->i] != save)
		vars->i++;
	if (vars->cmd[vars->i] == '\'' || vars->cmd[vars->i] == '"')
	{
		vars->i++;
		if (vars->flag == 1)
		{
			(*cmd)->cmd = ft_substr(vars->cmd, vars->tmp, vars->i - vars->tmp);
			(*cmd)->spaceafter = hasspaceafter(vars->cmd, vars->tmp - 1);
			vars->flag = 0;
		}
		else
		{
			if(save == '"')
				ft_newnode(cmd, ft_substr(vars->cmd, vars->tmp, vars->i - vars->tmp), hasspaceafter(vars->cmd, vars->tmp - 1));
			else
				ft_newnode(cmd, ft_substr(vars->cmd, vars->tmp, vars->i - vars->tmp), hasspaceafter(vars->cmd, vars->tmp - 1));
		}
		return (ft_switch(cmd, vars));
	}
	else
		return (printf("syntax error\n"), 1);
	return (0);
}
