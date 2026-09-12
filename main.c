#include "vbc.h"

int	main(int argc, char **argv)
{
	t_token	*tokens;
	int		result;
	int		status;

	if (argc == 2)
		status = tokenize(argv[1], &tokens);
	else
		status = tokenize("", &tokens);
	if (status == 0)
		return (1);
	if (status == -1)
		return (1);
	status = evaluate(tokens, &result);
	free(tokens);
	if (status == 0)
		return (1);
	if (status == -1)
		return (1);
	printf("%d\n", result);
	return (0);
}
