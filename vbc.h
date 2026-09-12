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

typedef struct s_parser
{
	t_token	*tokens;
	int		pos;
}	t_parser;

int		tokenize(const char *str, t_token **out);
int		parse_expr(t_parser *p, int *result);
int		parse_term(t_parser *p, int *result);
int		parse_factor(t_parser *p, int *result);
void	print_unexpected(t_token *t);

#endif
