/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 17:56:13 by julcleme          #+#    #+#             */
/*   Updated: 2026/02/10 11:37:39 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "data_structures.h"
#include "monitor.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

int	cleanup(t_args *args, pthread_mutex_t *dongles,
	t_scheduler *scheduler, t_coder_config *conf)
{
	int	i;

	if (conf && scheduler)
		stop_coders(conf);
	i = 0;
	while (conf && i < args->nb_coders)
		pthread_mutex_destroy(&conf[i++].time_mutex);
	if (conf)
		free(conf);
	i = 0;
	while (dongles && i < args->nb_coders)
		pthread_mutex_destroy(&dongles[i++]);
	if (dongles)
		free(dongles);
	if (!scheduler)
		return (0);
	pthread_mutex_destroy(&scheduler->mutex);
	pthread_cond_destroy(&scheduler->cond);
	free_lst(scheduler->queue);
	if (scheduler->dongle_state)
		free(scheduler->dongle_state);
	if (scheduler->dongle_release)
		free(scheduler->dongle_release);
	return (0);
}

static int	init_scheduler(pthread_mutex_t **dongles,
	t_args *args, t_coder_config **conf,
	t_scheduler *sch)
{
	*dongles = malloc(sizeof(pthread_mutex_t) * args->nb_coders);
	*conf = malloc(sizeof(t_coder_config) * args->nb_coders);
	if (!*conf || !*dongles)
		return (cleanup(args, *dongles, 0, *conf),
			puterr("Allocation failed\n"));
	if (pthread_mutex_init(&sch->mutex, 0) != 0)
		return (cleanup(args, *dongles, 0, *conf),
			puterr("Mutex init failed\n"));
	if (pthread_cond_init(&sch->cond, 0) != 0)
	{
		pthread_mutex_destroy(&sch->mutex);
		return (cleanup(args, *dongles, 0, *conf),
			puterr("Cond init failed\n"));
	}
	sch->stop_simulation = 0;
	sch->dongle_state = malloc(sizeof(int) * args->nb_coders);
	sch->dongle_release = malloc(sizeof(size_t) * args->nb_coders);
	if (!sch->dongle_state || !sch->dongle_release)
		return (cleanup(args, *dongles, sch, *conf),
			puterr("Allocation failed\n"));
	memset(sch->dongle_state, 0, sizeof(int) * args->nb_coders);
	memset(sch->dongle_release, 0, sizeof(size_t) * args->nb_coders);
	memset(*conf, 0, sizeof(t_coder_config) * args->nb_coders);
	log_message(0, -1);
	return (0);
}

static int	coders_init(t_args *args, pthread_mutex_t *dongles,
	t_coder_config *conf, t_scheduler *sch)
{
	int	i;

	i = 0;
	while (i < args->nb_coders)
	{
		conf[i].id = i + 1;
		conf[i].dongles = dongles;
		conf[i].state = COMPILE;
		conf[i].program_args = *args;
		conf[i].scheduler_mutex = sch;
		if (pthread_mutex_init(&conf[i].time_mutex, 0)
			|| pthread_mutex_init(&dongles[i++], 0))
			return (1);
	}
	return (0);
}

static int	init_threads(t_coder_config *conf, t_args *args)
{
	pthread_t	monitor;
	int			i;
	int			failed_idx;

	if (pthread_create(&monitor, 0, monitor_thread, &conf))
		return (puterr("Could not initialise monitor thread\n"));
	i = -1;
	failed_idx = -1;
	while (++i < args->nb_coders)
	{
		if (pthread_create(&(conf[i].thread), 0, coder_thread, &conf[i]))
		{
			failed_idx = i + stop_coders(conf) * puterr("Threads failed\n");
			break ;
		}
	}
	i = 0;
	while (i < args->nb_coders && (failed_idx == -1 || (i < failed_idx)))
	{
		if (pthread_join(conf[i++].thread, 0))
			return (puterr("Could not join coder threads\n"));
	}
	if (pthread_join(monitor, 0))
		return (puterr("Could not join monitor thread\n"));
	return (failed_idx != -1);
}

int	main(int argc, char **argv)
{
	t_args			args;
	t_coder_config	*conf;
	t_scheduler		sch;
	pthread_mutex_t	*dongles;

	if (argc != 9)
		return (puterr("Usage: ./codexion <nb_coders> <time_to_burnout> \
<time_to_compile> <time_to_debug> \
<time_to_refactor> <nb_compiles_required> \
<dongle_cooldown> <scheduler>\n"));
	if (parse_args(&args, argv) || init_scheduler(&dongles, &args, &conf, &sch))
		return (1);
	if (coders_init(&args, dongles, conf, &sch))
		return (1 + cleanup(&args, dongles, &sch, conf));
	sch.queue = create_lst(args.nb_coders, conf);
	if (!sch.queue)
		return (cleanup(&args, dongles, &sch, conf), \
puterr("Cannot allocate dongle release time\n"));
	init_threads(conf, &args);
	return (cleanup(&args, dongles, &sch, conf));
}
