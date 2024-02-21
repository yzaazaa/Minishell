/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asnaji <asnaji@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/02 14:11:54 by asnaji            #+#    #+#             */
/*   Updated: 2024/02/21 16:50:27 by asnaji           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# include <stdlib.h>
# include <limits.h>
# include <stdio.h>
# include <curses.h>
# include <term.h>
# include <string.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <unistd.h>
# include <signal.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <dirent.h>
# include <sys/ioctl.h>
# include <sys/stat.h>

# define FORK_ERROR "turboshell: fork: Resource temporarily unavailable\n"
# define PIPE_ERROR "pipe failed\n"


enum e_type
{
	TOKEN_EXPR,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_REDIR_APPEND,
	TOKEN_HEREDOC,
	TOKEN_PIPE,
	TOKEN_AND,
	TOKEN_OR,
	TOKEN_SPACE,
	TOKEN_D_Q,
	TOKEN_S_Q,
	TOKEN_DOLLAR,
	TOKEN_OPEN_BRACKET,
	TOKEN_CLOSED_BRACKET,
} ;

enum e_tree_type
{
	AND,
	OR,
	PIPE,
	CMD,
	REDIR_IN,
	REDIR_OUT,
	HEREDOC,
	REDIR_APPEND,
};


typedef struct s_token
{
	char			*cmd;
	int				type;
	int				visited;
	int				for_heredoc;
	int 			heredocfd;
	int				spaceafter;
	struct s_token	*prev;
	struct s_token	*next;
}				t_token;

typedef struct s_cmd
{
	char			*cmd;
	int				fd[2];
	int				word;
	int				spaceafter;
	int				heredocfd;
	int				expandheredoc;
	int				expandwildcard;
	int				ambiguous;
	struct s_cmd	*next;
}				t_cmd;

typedef struct s_args
{
	char			*cmd;
	int				spaceafter;
	int				expand;
	struct s_args	*next;
}				t_args;

typedef struct s_list
{
	void			*data;
	int				is_matching;
	struct s_list	*next;
	struct s_list	*prev;
	int				size;
}				t_list;

typedef struct s_tree
{
	char				*data;
	int					fd[2];
	t_cmd				*next;
	struct s_tree		*left;
	struct s_tree		*right;
	enum e_tree_type	tree_type;
}				t_tree;

typedef struct s_env
{
	int				size;
	char			*key;
	char			*value;
	struct s_env	*next;
	struct s_env	*prev;
	char			*pwd;
	int				env_null;
}				t_env;

typedef struct s_vars
{
	char			*cmd;
	int				flag;
	int				tmp;
	int				i;
	t_env			*env;
}				t_vars;

typedef struct s_syntax
{
	int initflag;
	int openc;
	int closedc;
}				t_syntax;

typedef struct s_gc
{
	void		*mallocedptr;
	int			type;
	struct s_gc	*next;
	struct s_gc	*prev;
}				t_gc;

typedef struct s_margs
{
	char	*buffer;
	int		word;
	int		spaceafter;
	int		flag;
	int		expand;
	int		heredocfd;
	int		heredocexpand;
	int		ambiguos;
}				t_margs;

typedef struct s_ncmdlst
{
	char *buffer;
	int		word;
	t_cmd *save;
	int ambiguous;
	int flag;
	t_cmd *save1;
}				t_ncmdlst;

typedef struct s_fd_list
{
	int fd;
	struct s_fd_list *next;
}				t_fd_list;

typedef struct s_jn_args
{
	int		flag;
	int		word;
	int		expand;
	char	*buffer;
	int		spaceafter;
}				t_jn_args;

typedef struct s_pattern
{
	int	i;
	int	j;
	int	last_asterisk;
	int	match;
	int	len_pattern;
	int	len_str;
}				t_pattern;

enum e_garbagecollector
{
	EVERYTHING,
	COMMAND,
	ENV,
	OTHERS,
};

void	*rad_malloc(size_t size, int flag, int type);
void	handle_input(t_token **cmd, char *str, t_env *env);
int		check_syntax_error(t_token **cmd);
int		checkdelimiter(int c);
int		ft_bracket(t_token **cmd, t_vars *vars);
int		ft_dollarsign(t_token **cmd, t_vars *vars);
int		array_len(char **arr);
size_t	ft_strlen(const char *s);
char	*ft_substr(char *s, size_t start, size_t len);
char	*ft_strdup(char *s1);
char	*ft_strdup_len(char *s1, int len);
char	*ft_strdup_del(char *s1);
size_t	ft_strlen(const char *s);
void	ft_newnode(t_token **cmd, char *value, int spaceafter);
void	ft_free_cmd(t_token *lst);
t_token	*init_token();
int		ft_isalpha(int c);
void	wrerror(char *str);
int		ft_isalnum(int c);
int		ft_atoi(const char *str);
int		ft_char(t_token **cmd, t_vars *vars);
void	give_state_and_type(t_token **cmd);
int		getlimitertoken(char c, char f);
int		getlimitertoken1(char c, char f);
int		look_for_char(char *command, int i);
t_token	*join_args_ifspace(t_token *cmd);
int		ft_separator(t_token **cmd, t_vars *vars);
int		ft_space(t_token **cmd, t_vars *vars);
int		ft_quote(t_token **cmd, t_vars *vars);
int		ft_switch(t_token **cmd, t_vars *vars);
int		hasspaceafter(char *str, int i);
t_tree	*make_node(t_token **cmd, int flag);
t_tree	*make_command(t_token *token);
t_tree	*search_pipe(t_token *token);
t_tree	*search_logical_operator(t_token *token);
t_token	*skip_brackets_next(t_token *token, int *is_brackets);
t_token	*skip_brackets_prev(t_token *token);
void	free_tree(t_tree **root);
t_tree	*make_node(t_token **cmd, int flag);
t_tree	*check_token(t_token **token, int *flag);
void	join_data(t_cmd *cmd, t_token **token);
t_cmd	*make_cmd(t_token *token);
void	add_cmd(t_cmd **cmd, t_token *token);
int		ft_isspace(char c);
int		ft_isquote(char c);
int		ft_alphanum(char c);
int		islimiter(int c);
int		islimiter1(int c);
int		islimiter2(int c);
int		isredirection(int c);
int		is_builtin(char *cmd);
int		getlastinfile(t_cmd *cmd, t_env *env);
int		getlastoutfile(t_cmd *cmd);
void	changeoutfile(int fd);
void	changeinfile(int fd);
char	**ft_split(char const *s, char c);
char	**ft_split_spaces(char const *s);
size_t	ft_strlcpy(char *dst, const char *src, size_t dstsize);
char	*ft_strjoin(char *s1, char *s2);
char	*get_working_path(char **envp, char *command);
char	*get_path_line(char **envp);
void	ft_pipe(char **av, char **envp, char *command, char *command2, char **commandargs1, char **commandargs2);
char	*ft_strtrim(char const *s1, char const *set);
int		ft_strncmp_rev(const char *s1, const char *s2, size_t n);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
int		ft_strcmp(const char *s1, const char *s2);
char	*ft_strchr(const char *str, int c);
void	ft_putstr(char *str);
void	ft_free_array(char **array);
void	free_env(t_env *env);
void	get_pwd(t_env *env);
void	print_env(t_env *env, int flag);
t_env	*arr_to_env(char **env);
void	add_env(t_env **env, char *key, char *value);
char	**env_to_arr(t_env *env);
char	*get_value_env(t_env *env, char *key);
void	change_value_env(t_env **env, char *key, char *value);
void	change_value_env_check_null(t_env **env, char *key, char *value);
void	del_node_env(t_env **env, char *key);
t_env	*get_node_env(t_env *env, char *key);
int		key_exist_env(t_env *env, char *key);
t_env	*make_env(char *key, char *value);
t_env	*get_env();
char	*expand(t_env *env, char *key);
int		unset(char **args, t_env **envp);
int		ft_export(char **args, t_env **env);
char	*get_points(char *str);
int		chdir_home(char **args, t_env **env);
int		dir_not_found(char **args);
int		dir_removed(char **args, t_env **env, char *dir);
int		chdir_dash(char **args, t_env **env);
int		treat_special_cases(char **args, t_env **env);
int		ft_cd(char **args, t_env **env);
int		ft_echo(char **args);
void	signal_handler(int signum);
void	signal_handler_heredoc(int signum);
int		heredocendedwith_c(int state, int flag);
void	print_start_message(void);
char	**ft_clone_env(char **env);
int		heredoc(char *str);
void	exec_heredoc(char *str);
void	find_node_to_execute(t_tree *root, t_env *env);
int		one_command_execution(t_tree *node, t_env *env);
int 	andorexecution(t_tree *root, t_env *env);
int		pipe_execution(t_tree *node, t_env *env);
int		pipeexecution1(t_tree *node, t_tree *save, t_env *env);
char	*expand(t_env *env, char *key);
int		improvedpipeexecution(t_tree *node, t_env *env);
int		improvedpipeexecution1(t_tree *node, t_env *env, t_tree *save);
int 	checkwritepermissions(char *str, int *outfile);
int		checkreadpermissions(char *str, int *infile);
char	*ft_itoa(int n);
char	*quotes_toexpression(char *str, t_env *env);
char	*heredoc_expanding(char *str, t_env *env);
int		exitstatus(int newstatus, int flag);
t_cmd	*new_cmd_list(t_cmd *root , t_env *env);
char	*argextraction(t_cmd *token, t_env *env);
char	**var_toarray(char *token, t_env *env);
int		count_var_args(char *token, t_env *env);
int		args_size(t_cmd *temp, t_env *env);
int		check_expanded_var(char *str, t_env *env);
void	firstit1(char **args, int *i, char **tmp);
void	ft_firstit(t_cmd *token, int *i, t_env *env, char **args);
void	ft_secondit1(char **args, int *i, char **tmp);
void	ft_secondit(t_cmd *token, int *i, t_env *env, char **args);
char	**join_args1(t_cmd *root, t_env *env);
int		heredocshit(char *delimiter);
int		heredoc_expanded(int fd, t_env *env);
t_cmd	*get_command_start(t_cmd *node);
int		builtinexec(char **args, t_env **env, int infile, int outfile);
void	ft_exit(char **args);
void	lst_add_node(t_list **lst, void *data);
t_list	*get_dirent(void);
int		len_arr_list(t_list *lst);
char	**wildcard(t_cmd *args);
char	**get_all_wildcards(t_cmd *args);
int		is_match(char *str, char *pattern);
t_list	*sort_list(t_list* lst, int (*cmp)(const char *, const char *));
void	free_list(t_list **lst);
int		count_len_matching(char *str, t_list *dirent);
t_list	*remove_hidden(t_list *dirent);
void	join_dirent(t_list *dirent);
char	**args_to_arr(t_cmd *args);
t_cmd	*make_args_lst(t_cmd *cmd, t_env *env);
void	getfds(t_cmd *cmd, t_env *env, int *infile, int *outfile);
void	make_args_node(t_cmd **args, t_margs **vars);
void	skip_redirections(t_cmd **temp);
void	makea_part2_2(t_cmd **cmd, t_cmd **new_lst, t_margs **vars, t_env *env);
void	makearpart1(t_cmd **cmd, t_cmd **new_lst, t_margs **vars, t_env *env);
t_cmd	*joined_args(t_cmd *args);
void	setvars_argsnode(t_margs **vars, t_cmd **newlst);
void	setexit(int status);
void	new_nodforquotes(t_token **cmd, t_vars *vars, char save);
void addfd(int fd, int flag);

#endif