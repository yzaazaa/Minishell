/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yzaazaa <yzaazaa@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 22:10:15 by yzaazaa           #+#    #+#             */
/*   Updated: 2024/01/16 02:19:32 by yzaazaa          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_tree	*new_node(void *data, enum e_tree_type tree_type)
{
	t_tree	*ret;

	ret = malloc(sizeof(t_tree));
	if (!ret)
		return (NULL);
	ret->data = data;
	ret->right = NULL;
	ret->left = NULL;
	ret->tree_type = tree_type;
	return (ret);
}

void	add_left_child(t_tree **tree, void *data, enum e_tree_type tree_type)
{
	t_tree	*new_left;

	new_left = new_node(data, tree_type);
	if (!new_left)
		return ;
	if (!*tree)
	{
		*tree = new_left;
		return ;
	}
	(*tree)->left = new_left;
}

void	add_right_child(t_tree **tree, void *data, enum e_tree_type tree_type)
{
	t_tree	*new_right;

	new_right = new_node(data, tree_type);
	if (!new_right)
		return ;
	if (!*tree)
	{
		*tree = new_right;
		return ;
	}
	(*tree)->right = new_right;
}

void	make_tree_right(t_tree **tree, t_cmd *cmd)
{
	t_cmd	*save;

	save = cmd;
	while (cmd && cmd->visited != 1)
	{
		if (cmd->type == TOKEN_PIPE)
		{
			cmd->visited = 1;
			add_right_child(tree, cmd, PIPE);
			if (cmd->next)
				make_tree_right(&((*tree)->right), cmd->next);
			if (cmd->prev)
				make_tree_left(&((*tree)->left), cmd->prev);
		}
		cmd = cmd->next;
	}
	cmd = save;
	while (cmd && cmd->visited != 1)
	{
		if (cmd->type == TOKEN_EXPR)
		{
			cmd->visited = 1;
			add_right_child(tree, cmd, CMD);
			if (cmd->next)
				make_tree_right(&((*tree)->right), cmd->next);
			if (cmd->prev)
				make_tree_left(&((*tree)->left), cmd->prev);
		}
		cmd = cmd->next;
	}
}

void	make_tree_left(t_tree **tree, t_cmd *cmd)
{
	t_cmd	*save;

	save = cmd;
	while (cmd && cmd->visited != 1)
	{
		if (cmd->type == TOKEN_AND)
		{
			cmd->visited = 1;
			add_left_child(tree, cmd, AND);
			if (cmd->next)
				make_tree_right(&((*tree)->right), cmd->next);
			if (cmd->prev)
				make_tree_left(&((*tree)->left), cmd->prev);
		}
		else if (cmd->type == TOKEN_OR)
		{
			cmd->visited = 1;
			add_left_child(tree, cmd, OR);
			if (cmd->next)
				make_tree_right(&((*tree)->right), cmd->next);
			if (cmd->prev)
				make_tree_left(&((*tree)->left), cmd->prev);
		}
		cmd = cmd->prev;
	}
	cmd = save;
	while (cmd && cmd->visited != 1)
	{
		if (cmd->type == TOKEN_PIPE)
		{
			cmd->visited = 1;
			add_left_child(tree, cmd, PIPE);
			if (cmd->next)
				make_tree_right(&((*tree)->right), cmd->next);
			if (cmd->prev)
				make_tree_left(&((*tree)->left), cmd->prev);
		}
		cmd = cmd->prev;
	}
	cmd = save;
	while (cmd && cmd->visited != 1)
	{
		if (cmd->type == TOKEN_EXPR)
		{
			cmd->visited = 1;
			add_left_child(tree, cmd, CMD);
			if (cmd->next)
				make_tree_right(&((*tree)->right), cmd->next);
			if (cmd->prev)
				make_tree_left(&((*tree)->left), cmd->prev);
		}
		cmd = cmd->prev;
	}
}

int and_or_in_cmd(t_cmd *cmd)
{
	t_cmd *curr;

	curr = cmd;
	while(curr)
	{
		if(curr->type == TOKEN_AND || curr->type == TOKEN_OR)
			return 1;
		curr = curr->prev;
	}
	return 0;
}

int pipe_in_cmd(t_cmd *cmd)
{
	t_cmd *curr;

	curr = cmd;
	while(curr)
	{
		if(curr->type == TOKEN_PIPE)
			return 1;
		curr = curr->prev;
	}
	return 0;
}

void rotationDroite(t_tree **T)
{
    if(*T==NULL || (*T)->left==NULL)
        return;
    t_tree *temp = (*T)->left;
    (*T)->left = temp->right;
    temp->right = *T;
    *T = temp;
    return;
}

t_tree	*make_tree(t_cmd *cmd)
{
	t_tree	*first_node_right;
	t_tree	*first_node_left;
	t_tree	*right;
	t_tree	*left;

	right = NULL;
	left = NULL;
	while (cmd->next)
		cmd = cmd->next;
	if (and_or_in_cmd(cmd))
	{
		while (cmd && cmd->visited != 1)
		{
			if (cmd->type == TOKEN_AND)
			{
				cmd->visited = 1;
				add_left_child(&right, cmd, AND);
				add_left_child(&left, cmd, AND);
				first_node_right = right;
				first_node_left = left;
				if (cmd->next)
					make_tree_right(&right, cmd->next);
				if (cmd->prev)
					make_tree_left(&left, cmd->prev);
			}
			else if (cmd->type == TOKEN_OR)
			{
				cmd->visited = 1;
				add_left_child(&right, cmd, OR);
				add_left_child(&left, cmd, OR);
				first_node_right = right;
				first_node_left = left;
				if (cmd->next)
					make_tree_right(&right, cmd->next);
				if (cmd->prev)
					make_tree_left(&left, cmd->prev);
			}
			cmd = cmd->prev;
		}
	}
	else
	{ 
		if (pipe_in_cmd(cmd))
		{
			while (cmd && cmd->visited != 1)
			{
				if (cmd->type == TOKEN_PIPE)
				{
					cmd->visited = 1;
					add_left_child(&right, cmd, PIPE);
					add_left_child(&left, cmd, PIPE);
					first_node_right = right;
					first_node_left = left;
					if (cmd->next)
						make_tree_right(&right, cmd->next);
					if (cmd->prev)
						make_tree_left(&left, cmd->prev);
				}
				cmd = cmd->prev;
			}
		}
		else
		{
			while (cmd && cmd->visited != 1)
			{
				if (cmd->type == TOKEN_EXPR)
				{
					cmd->visited = 1;
					add_left_child(&right, cmd, CMD);
					add_left_child(&left, cmd, CMD);
					first_node_right = right;
					first_node_left = left;
					if (cmd->next)
						make_tree_right(&right, cmd->next);
					if (cmd->prev)
						make_tree_left(&left, cmd->prev);
				}
				cmd = cmd->prev;
			}
		}
	}
	t_tree	*head = new_node(first_node_left, first_node_left->tree_type);
	head->right = first_node_right->right;
	head->left = first_node_left->left;
	// rotationDroite(&head);
	return (head);
}
