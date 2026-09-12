#ifndef VBC_H
# define VBC_H

# include <stdlib.h>
# include <unistd.h>
# include <ctype.h>
# include <stdio.h>

typedef enum e_toktype
{
	TOK_NUM,
	TOK_PLUS,
	TOK_STAR,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_END
}	t_toktype;

typedef struct s_token
{
	t_toktype	type;
	int			value;
}	t_token;

typedef struct s_stack
{
	int			*vals;
	int			vtop;
	t_toktype	*ops;
	int			otop;
}	t_stack;

int		tokenize(const char *str, t_token **out);
int		evaluate(t_token *tokens, int *result);
void	print_unexpected(t_token *t);

#endif
