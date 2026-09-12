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

static int	count_tokens(t_token *tokens)
{
	int	n;

	n = 0;
	while (tokens[n].type != TOK_END)
		n++;
	return (n);
}

/*
** Phase 0: runs once, fully after tokenizing is done, over the whole
** array. It only checks the expression is well formed (every token
** shows up where the grammar allows it, every '(' finds a ')') and
** does not compute anything yet.
*/
static int	validate(t_token *tokens)
{
	int		expect_operand;
	int		depth;
	int		i;
	t_token	*t;

	expect_operand = 1;
	depth = 0;
	i = 0;
	while (1)
	{
		t = &tokens[i];
		if (t->type == TOK_NUM)
		{
			if (!expect_operand)
				return (print_unexpected(t), -1);
			expect_operand = 0;
		}
		else if (t->type == TOK_PLUS || t->type == TOK_STAR)
		{
			if (expect_operand)
				return (print_unexpected(t), -1);
			expect_operand = 1;
		}
		else if (t->type == TOK_LPAREN)
		{
			if (!expect_operand)
				return (print_unexpected(t), -1);
			depth++;
		}
		else if (t->type == TOK_RPAREN)
		{
			if (expect_operand || depth == 0)
				return (print_unexpected(t), -1);
			depth--;
			expect_operand = 0;
		}
		else
		{
			if (expect_operand || depth != 0)
				return (print_unexpected(t), -1);
			return (1);
		}
		i++;
	}
}

static int	find_type(t_token *arr, int len, t_toktype type)
{
	int	i;

	i = 0;
	while (i < len)
	{
		if (arr[i].type == type)
			return (i);
		i++;
	}
	return (-1);
}

/*
** Removes the operator at k and its right operand at k+1 (the left
** operand at k-1 already holds the combined value) by shifting
** everything after them one step to the left.
*/
static void	remove_pair(t_token *arr, int *len, int k)
{
	int	i;

	i = k;
	while (i + 2 < *len)
	{
		arr[i] = arr[i + 2];
		i++;
	}
	*len -= 2;
}

/*
** Phase 2: one loop that keeps collapsing the first "NUM * NUM" it
** finds until no '*' is left in the (now parenthesis-free) array.
*/
static void	solve_mult(t_token *arr, int *len)
{
	int	k;

	k = find_type(arr, *len, TOK_STAR);
	while (k != -1)
	{
		arr[k - 1].value = arr[k - 1].value * arr[k + 1].value;
		remove_pair(arr, len, k);
		k = find_type(arr, *len, TOK_STAR);
	}
}

/*
** Phase 3: same idea for '+', run only once every '*' is gone.
*/
static void	solve_add(t_token *arr, int *len)
{
	int	k;

	k = find_type(arr, *len, TOK_PLUS);
	while (k != -1)
	{
		arr[k - 1].value = arr[k - 1].value + arr[k + 1].value;
		remove_pair(arr, len, k);
		k = find_type(arr, *len, TOK_PLUS);
	}
}

static int	find_matching_open(t_token *arr, int close)
{
	int	i;

	i = close - 1;
	while (arr[i].type != TOK_LPAREN)
		i--;
	return (i);
}

/*
** Replaces the whole "( ... )" block tokens[i..j] with a single NUM
** token holding its already-computed value, shifting the remainder
** of the array to close the gap.
*/
static void	replace_paren(t_token *arr, int *len, int i, int j, int value)
{
	int	shift;
	int	k;

	shift = j - i;
	arr[i].type = TOK_NUM;
	arr[i].value = value;
	k = i + 1;
	while (k + shift < *len)
	{
		arr[k] = arr[k + shift];
		k++;
	}
	*len -= shift;
}

/*
** Phase 1: repeatedly locates the first ')' still in the array (its
** matching '(' is always the nearest one before it once the
** expression has been validated) and collapses everything inside it
** into a single value, using the same solve_mult/solve_add order on
** that isolated content before it gets folded back in.
*/
static int	solve_parens(t_token *tokens, int *len)
{
	int		j;
	int		i;
	int		k;
	int		seg_len;
	t_token	*tmp;

	j = find_type(tokens, *len, TOK_RPAREN);
	while (j != -1)
	{
		i = find_matching_open(tokens, j);
		seg_len = j - i - 1;
		tmp = malloc(sizeof(t_token) * seg_len);
		if (!tmp)
			return (0);
		k = 0;
		while (k < seg_len)
		{
			tmp[k] = tokens[i + 1 + k];
			k++;
		}
		solve_mult(tmp, &seg_len);
		solve_add(tmp, &seg_len);
		replace_paren(tokens, len, i, j, tmp[0].value);
		free(tmp);
		j = find_type(tokens, *len, TOK_RPAREN);
	}
	return (1);
}

/*
** Runs strictly after tokenize() has already read the whole
** expression: validate the finished array, then solve parentheses,
** then multiplications, then additions, each its own separate pass.
*/
int	evaluate(t_token *tokens, int *result)
{
	int	len;
	int	status;

	status = validate(tokens);
	if (status == -1)
		return (-1);
	len = count_tokens(tokens);
	status = solve_parens(tokens, &len);
	if (status == 0)
		return (0);
	solve_mult(tokens, &len);
	solve_add(tokens, &len);
	*result = tokens[0].value;
	return (1);
}
