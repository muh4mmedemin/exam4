#include "vbc.h"

void	print_unexpected(t_token *t)
{
	char	c;

	if (t->type == TOK_END)
	{
		printf("Unexpected end of input\n");
		return ;
	}
	if (t->type == TOK_NUM)
		c = '0' + t->value;
	else if (t->type == TOK_PLUS)
		c = '+';
	else if (t->type == TOK_STAR)
		c = '*';
	else if (t->type == TOK_LPAREN)
		c = '(';
	else
		c = ')';
	printf("Unexpected token '%c'\n", c);
}

/*
** factor := number | '(' expr ')'
** Handles the parenthesis: an opening paren makes us solve the whole
** sub-expression inside it before anything else, via recursion.
*/
int	parse_factor(t_parser *p, int *result)
{
	t_token	*t;

	t = &p->tokens[p->pos];
	if (t->type == TOK_NUM)
	{
		*result = t->value;
		p->pos++;
		return (1);
	}
	if (t->type == TOK_LPAREN)
	{
		p->pos++;
		if (!parse_expr(p, result))
			return (0);
		t = &p->tokens[p->pos];
		if (t->type != TOK_RPAREN)
		{
			print_unexpected(t);
			return (0);
		}
		p->pos++;
		return (1);
	}
	print_unexpected(t);
	return (0);
}

/*
** term := factor ('*' factor)*
** Solves every multiplication before returning to parse_expr, so '+'
** never gets a chance to run before the '*' around it.
*/
int	parse_term(t_parser *p, int *result)
{
	int	rhs;

	if (!parse_factor(p, result))
		return (0);
	while (p->tokens[p->pos].type == TOK_STAR)
	{
		p->pos++;
		if (!parse_factor(p, &rhs))
			return (0);
		*result = *result * rhs;
	}
	return (1);
}

/*
** expr := term ('+' term)*
** Solved last: additions are the outermost/lowest priority operation.
*/
int	parse_expr(t_parser *p, int *result)
{
	int	rhs;

	if (!parse_term(p, result))
		return (0);
	while (p->tokens[p->pos].type == TOK_PLUS)
	{
		p->pos++;
		if (!parse_term(p, &rhs))
			return (0);
		*result = *result + rhs;
	}
	return (1);
}
