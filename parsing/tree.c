/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaazaa <yzaazaa@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 22:10:15 by yzaazaa           #+#    #+#             */
/*   Updated: 2024/01/26 05:22:49 by yzaazaa          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

// If we want to skip the heredoc and its limiter

static void	skip_heredoc(t_token **token)
{
	while ((*token) && (*token)->type == TOKEN_HEREDOC)
		(*token) = (*token)->next->next;
}

t_tree	*make_command(t_token *token)
{
	t_tree	*node;
	t_cmd	*cmd;

	while (token->prev && token->prev->visited != 1)
		token = token->prev;
	skip_heredoc(&token);
	if (!token)
		return (NULL);
	node = make_node(&token, 0);
	node->next = make_cmd(token);
	cmd = node->next;
	token = token->next;
	while (token && token->visited != 1)
	{
		skip_heredoc(&token);
		if (token)
		{
			add_cmd(&cmd, token);
			token = token->next;
		}
	}
	return (node);
}

static t_tree	*choose_function(t_tree *node, t_token *token, int is_brackets)
{
	node = make_node(&token, 1);
	if (is_brackets)
		node->left = search_logical_operator(token->prev->prev);
	else
		node->left = make_command(token->prev);
	node->right = search_pipe(token->next);
	return (node);
}

t_tree	*search_pipe(t_token *token)
{
	t_token	*save;
	t_tree	*node;
	int		is_brackets;

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

t_tree	*search_logical_operator(t_token *token)
{
	t_token	*save;
	t_tree	*node;

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