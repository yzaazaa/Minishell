/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaazaa <yzaazaa@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/13 15:27:25 by yzaazaa           #+#    #+#             */
/*   Updated: 2024/02/06 17:04:01 by yzaazaa          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

static int	check_arg(char *str)
{
	int	i;

	i = 0;
	if (!ft_isalpha(str[i]) && str[i] != '_')
		return (0);
	i++;
	while (str[i])
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

int	unset(char **args, t_env **envp)
{
	int		i;
	t_env	*tmp;
	t_env	*tmp2;

	i = 1;
	while (args[i])
	{
		if (!check_arg(args[i]))
		{
			write(2, "unset: `", 8);
			write(2, args[i], ft_strlen(args[i]));
			write(2, "': not a valid identifier\n", 26);
			exitstatus(1, 1);
		}
		else
		{
			tmp = *envp;
			while (tmp)
			{
				if (!ft_strcmp(tmp->key, args[i]))
				{
					if (tmp->prev)
						tmp->prev->next = tmp->next;
					tmp2 = tmp;
					tmp = tmp->next;
					free(tmp2->key);
					free(tmp2->value);
					free(tmp2);
					break ;
				}
				else
					tmp = tmp->next;
			}
		}
		i++;
	}
	return (0);
}
