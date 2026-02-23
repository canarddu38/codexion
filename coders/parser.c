/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 01:13:19 by julcleme          #+#    #+#             */
/*   Updated: 2026/02/23 08:57:27 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "data_structures.h"
#include "parser.h"

int	puterr(char *str)
{
	fprintf(stderr, "\033[31mERROR\033[0m: %s", str);
	return (1);
}

static int	isnum(char *str)
{
	int		j;
	char	*max;

	j = 0;
	max = "2147483647";
	while (str[j] && str[j] >= '0' && str[j] <= '9')
		j++;
	if (j > 10 || j == 0)
		return (0);
	if (j == 10)
	{
		j = 0;
		while (j < 10 && str[j] <= max[j])
			j++;
		if (j < 10)
			return (0);
	}
	return (!str[j]);
}

int	parse_args(t_args *args, char **argv)
{
	int	i;

	i = 1;
	while (i < 8)
	{
		if (!isnum(argv[i]))
			return (puterr("Invalid argument given as parameter\n"));
		i++;
	}
	args->nb_coders = atoi(argv[1]);
	if (args->nb_coders < 1)
		return (puterr("Number of coders cannot be null\n"));
	args->time_to_burnout = atoi(argv[2]);
	args->time_to_compile = atoi(argv[3]);
	args->time_to_debug = atoi(argv[4]);
	args->time_to_refactor = atoi(argv[5]);
	args->nb_compiles_required = atoi(argv[6]);
	args->dongle_cooldown = atoi(argv[7]);
	if (strcmp("edf", argv[8]) == 0)
		args->scheduler = SCHEDULER_EDF;
	else if (strcmp("fifo", argv[8]) == 0)
		args->scheduler = SCHEDULER_FIFO;
	else
		return (puterr("Invalid scheduler: fifo or edf only\n"));
	return (0);
}
