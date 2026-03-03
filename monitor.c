/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 17:56:34 by julcleme          #+#    #+#             */
/*   Updated: 2026/02/10 11:30:20 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "monitor.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

static int	check_stop(t_coder_config *conf, int i)
{
	pthread_mutex_lock(&conf[i].scheduler_mutex->mutex);
	if (conf[i].scheduler_mutex->stop_simulation)
	{
		pthread_mutex_unlock(&conf[i].scheduler_mutex->mutex);
		return (1);
	}
	pthread_mutex_unlock(&conf[i].scheduler_mutex->mutex);
	return (0);
}

static int	check_burnout(t_coder_config *conf,
	int i, int current_time)
{
	int	last_time;

	pthread_mutex_lock(&conf[i].time_mutex);
	last_time = conf[i].last_time_compiled;
	pthread_mutex_unlock(&conf[i].time_mutex);
	if (current_time - last_time
		>= conf->program_args.time_to_burnout)
	{
		pthread_mutex_lock(&conf[i].scheduler_mutex->mutex);
		conf[i].scheduler_mutex->stop_simulation = 1;
		pthread_cond_broadcast(&conf[i].scheduler_mutex->cond);
		pthread_mutex_unlock(&conf[i].scheduler_mutex->mutex);
		log_message("burned out", conf[i].id);
		return (1);
	}
	return (0);
}

int	stop_coders(t_coder_config *conf)
{
	pthread_mutex_lock(&conf->scheduler_mutex->mutex);
	conf->scheduler_mutex->stop_simulation = 1;
	pthread_cond_broadcast(&conf->scheduler_mutex->cond);
	pthread_mutex_unlock(&conf->scheduler_mutex->mutex);
	return (0);
}

static void	queue_move(t_coder_config	*conf)
{
	pthread_mutex_lock(&conf->scheduler_mutex->mutex);
	pthread_cond_broadcast(&conf->scheduler_mutex->cond);
	pthread_mutex_unlock(&conf->scheduler_mutex->mutex);
	usleep(10);
}

void	*monitor_thread(void *arg)
{
	t_coder_config	*conf;
	int				i;
	int				current_time;
	int				finished;

	conf = *(t_coder_config **)arg;
	while (1)
	{
		finished = 1;
		i = -1;
		current_time = log_message(0, -1);
		while (++i < conf->program_args.nb_coders)
		{
			if (check_stop(conf, i) || check_burnout(conf, i, current_time))
				return (0);
			pthread_mutex_lock(&conf[i].time_mutex);
			if (conf[i].compiled < conf->program_args.nb_compiles_required)
				finished = 0;
			pthread_mutex_unlock(&conf[i].time_mutex);
		}
		if (finished)
			break ;
		queue_move(conf);
	}
	return ((void *)(long)stop_coders(conf));
}
