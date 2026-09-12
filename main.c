#include "vbc.h"

int	main(int argc, char **argv)
{
	t_token		*tokens;
	t_parser	p;
	int			result;
	int			status;

	if (argc == 2)
		status = tokenize(argv[1], &tokens);
	else
		status = tokenize("", &tokens);
	if (status == 0)
		return (1);
	if (status == -1)
		return (1);
	p.tokens = tokens;
	p.pos = 0;
	if (!parse_expr(&p, &result))
	{
		free(tokens);
		return (1);
	}
	if (p.tokens[p.pos].type != TOK_END)
	{
		print_unexpected(&p.tokens[p.pos]);
		free(tokens);
		return (1);
	}
	printf("%d\n", result);
	free(tokens);
	return (0);
}
