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

static int	precedence(t_toktype op)
{
	if (op == TOK_STAR)
		return (2);
	return (1);
}

/*
** Pops the operator on top of the stack together with its two
** operands, solves it and pushes the result back. Every '*' gets
** popped (and solved) before any '+' below it, since '*' always has
** the higher precedence value.
*/
static void	apply_top(t_stack *s)
{
	int			rhs;
	int			lhs;
	t_toktype	op;

	rhs = s->vals[--s->vtop];
	lhs = s->vals[--s->vtop];
	op = s->ops[--s->otop];
	if (op == TOK_PLUS)
		s->vals[s->vtop++] = lhs + rhs;
	else
		s->vals[s->vtop++] = lhs * rhs;
}

static int	fail(t_stack *s, t_token *t)
{
	print_unexpected(t);
	free(s->vals);
	free(s->ops);
	return (-1);
}

static int	count_tokens(t_token *tokens)
{
	int	n;

	n = 0;
	while (tokens[n].type != TOK_END)
		n++;
	return (n);
}

/*
** Iterative (no recursion) shunting-yard style evaluator: one left to
** right pass over the token array, using two explicit stacks instead
** of the call stack. '(' is simply pushed and later popped by its
** ')', which is exactly "solve the parenthesis"; '*' is popped and
** solved before a lower priority '+' waiting below it on the operator
** stack, which is "solve the multiplications, then the additions".
*/
int	evaluate(t_token *tokens, int *result)
{
	t_stack	s;
	int		i;
	int		expect_operand;
	t_token	*t;

	s.vals = malloc(sizeof(int) * (count_tokens(tokens) + 1));
	s.ops = malloc(sizeof(t_toktype) * (count_tokens(tokens) + 1));
	if (!s.vals || !s.ops)
	{
		free(s.vals);
		free(s.ops);
		return (0);
	}
	s.vtop = 0;
	s.otop = 0;
	expect_operand = 1;
	i = 0;
	while (1)
	{
		t = &tokens[i];
		if (t->type == TOK_NUM)
		{
			if (!expect_operand)
				return (fail(&s, t));
			s.vals[s.vtop++] = t->value;
			expect_operand = 0;
		}
		else if (t->type == TOK_PLUS || t->type == TOK_STAR)
		{
			if (expect_operand)
				return (fail(&s, t));
			while (s.otop > 0 && s.ops[s.otop - 1] != TOK_LPAREN
				&& precedence(s.ops[s.otop - 1]) >= precedence(t->type))
				apply_top(&s);
			s.ops[s.otop++] = t->type;
			expect_operand = 1;
		}
		else if (t->type == TOK_LPAREN)
		{
			if (!expect_operand)
				return (fail(&s, t));
			s.ops[s.otop++] = TOK_LPAREN;
		}
		else if (t->type == TOK_RPAREN)
		{
			if (expect_operand)
				return (fail(&s, t));
			while (s.otop > 0 && s.ops[s.otop - 1] != TOK_LPAREN)
				apply_top(&s);
			if (s.otop == 0)
				return (fail(&s, t));
			s.otop--;
			expect_operand = 0;
		}
		else
		{
			if (expect_operand)
				return (fail(&s, t));
			while (s.otop > 0)
			{
				if (s.ops[s.otop - 1] == TOK_LPAREN)
					return (fail(&s, t));
				apply_top(&s);
			}
			*result = s.vals[0];
			free(s.vals);
			free(s.ops);
			return (1);
		}
		i++;
	}
}
