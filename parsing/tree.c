/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaazaa <yzaazaa@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 22:10:15 by yzaazaa           #+#    #+#             */
/*   Updated: 2024/01/21 23:01:43 by yzaazaa          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	is_redirection(t_cmd *token)
{
	if (token->type == TOKEN_REDIR_IN || token->type == TOKEN_REDIR_OUT
		|| token->type == TOKEN_HEREDOC || token->type == TOKEN_REDIR_APPEND)
		return (1);
	return (0);
}

t_tree	*make_command(t_cmd *token)
{
	t_tree	*root;
	t_tree	*node;
	int		flag;

	flag = 0;
	node = check_token(&token, &flag);
	root = node;
	while (token && token->visited != 1)
	{
		if (flag)
		{
			add_right_child(&node, &token, 0);
			node = node->right;
			token = token->next;
		}
		join_data(node, &token);
		if (token && token->visited != 1 && is_redirection(token))
		{
			add_right_child(&node, &token, 0);
			flag = 1;
			node = node->right;
			token = token->next;
		}
	}
	return (root);
}

static t_tree	*choose_function(t_tree *node, t_cmd *token, int is_brackets)
{
	node = make_node(&token, 1);
	if (is_brackets)
		node->left = search_logical_operator(token->prev->prev);
	else
		node->left = make_command(token->prev);
	node->right = search_pipe(token->next);
	return (node);
}

t_tree	*search_pipe(t_cmd *token)
{
	t_cmd	*save;
	t_tree	*node;
	int		is_brackets;
	int		nb_brackets;

	node = NULL;
	if (!token)
		return (node);
	is_brackets = 0;
	while (token && token->visited != 1)
	{
		token = skip_brackets_next(token, &is_brackets);
		save = token;
		if (token && token->visited != 1 && token->type == TOKEN_CLOSED_BRACKET)
			token->visited = 1;
		if (token && token->visited != 1 && token->type == TOKEN_PIPE)
			return (choose_function(node, token, is_brackets));
		if (token)
			token = token->next;
	}
	if (is_brackets)
		return (search_logical_operator(save->prev));
	return (make_command(save));
}

t_tree	*search_logical_operator(t_cmd *token)
{
	t_cmd	*save;
	t_tree	*node;
	int		nb_brackets;

	node = NULL;
	while (token && token->visited != 1)
	{
		token = skip_brackets_prev(token);
		save = token;
		if (token && token->visited != 1 && token->type == TOKEN_OPEN_BRACKET)
			token = token->prev;
		if (token && token->visited != 1
			&& (token->type == TOKEN_AND || token->type == TOKEN_OR))
		{
			node = make_node(&token, 1);
			node->right = search_pipe(token->next);
			node->left = search_logical_operator(token->prev);
			return (node);
		}
		if (token)
			token = token->prev;
	}
	return (search_pipe(save));
}