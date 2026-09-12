#include "vbc.h"

static int	str_len(const char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

/*
** Reads the whole expression once and stores every character as a
** token in a struct array. Returns 1 on success, 0 on allocation
** failure (syscall failure -> caller must exit 1 silently), -1 when
** an invalid character was found (error message already printed).
*/
int	tokenize(const char *str, t_token **out)
{
	t_token	*tokens;
	int		len;
	int		i;
	int		n;

	len = str_len(str);
	tokens = malloc(sizeof(t_token) * (len + 1));
	if (!tokens)
		return (0);
	i = 0;
	n = 0;
	while (str[i])
	{
		if (isdigit((unsigned char)str[i]))
		{
			tokens[n].type = TOK_NUM;
			tokens[n].value = str[i] - '0';
		}
		else if (str[i] == '+')
			tokens[n].type = TOK_PLUS;
		else if (str[i] == '*')
			tokens[n].type = TOK_STAR;
		else if (str[i] == '(')
			tokens[n].type = TOK_LPAREN;
		else if (str[i] == ')')
			tokens[n].type = TOK_RPAREN;
		else
		{
			printf("Unexpected token '%c'\n", str[i]);
			free(tokens);
			return (-1);
		}
		n++;
		i++;
	}
	tokens[n].type = TOK_END;
	*out = tokens;
	return (1);
}
