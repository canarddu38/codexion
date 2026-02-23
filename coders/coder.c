/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 18:28:42 by julcleme          #+#    #+#             */
/*   Updated: 2026/02/10 11:36:55 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "data_structures.h"
#include "coder_utils.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int	get_deadline_edf(t_coder_config *coder)
{
	return (coder->last_time_compiled + coder->program_args.time_to_burnout);
}

static int	coder_compile(t_coder_config *conf,
	pthread_mutex_t *first_mutex,
	pthread_mutex_t *second_mutex, bool one_coder)
{
	scheduler_wait(conf);
	if (conf->scheduler_mutex->stop_simulation)
		return (1);
	pthread_mutex_lock(first_mutex);
	log_message("has taken a dongle", conf->id);
	while (one_coder)
	{
		if (conf->scheduler_mutex->stop_simulation)
			return (1);
	}
	pthread_mutex_lock(second_mutex);
	log_message("has taken a dongle", conf->id);
	conf->last_time_compiled = log_message("is compiling", conf->id);
	usleep(conf->program_args.time_to_compile * 1000);
	pthread_mutex_unlock(first_mutex);
	pthread_mutex_unlock(second_mutex);
	scheduler_signal(conf);
	conf->compiled++;
	return (0);
}

static int	coder_iteration(t_coder_config *conf,
	pthread_mutex_t *first_mutex,
	pthread_mutex_t *second_mutex, bool one_coder)
{
	pthread_mutex_lock(&conf->scheduler_mutex->mutex);
	if (conf->scheduler_mutex->stop_simulation)
	{
		pthread_mutex_unlock(&conf->scheduler_mutex->mutex);
		return (1);
	}
	pthread_mutex_unlock(&conf->scheduler_mutex->mutex);
	if (conf->state == COMPILE)
		return (coder_compile(conf, first_mutex, second_mutex, one_coder));
	else if (conf->state == DEBUG)
	{
		log_message("is debugging", conf->id);
		usleep(conf->program_args.time_to_debug * 1000);
		conf->state = REFACTOR;
	}
	else if (conf->state == REFACTOR)
	{
		log_message("is refactoring", conf->id);
		usleep(conf->program_args.time_to_refactor * 1000);
		conf->request_time = log_message(0, -1);
		pthread_mutex_lock(&conf->scheduler_mutex->mutex);
		conf->state = COMPILE;
		pthread_mutex_unlock(&conf->scheduler_mutex->mutex);
	}
	return (0);
}

void	*coder_thread(void	*arg)
{
	t_coder_config	*conf;
	pthread_mutex_t	*first_mutex;
	pthread_mutex_t	*second_mutex;
	int				first_idx;
	int				second_idx;

	conf = (t_coder_config *)arg;
	first_idx = conf->id - 1;
	second_idx = conf->id % conf->program_args.nb_coders;
	if (first_idx < second_idx)
	{
		first_mutex = &conf->dongles[first_idx];
		second_mutex = &conf->dongles[second_idx];
	}
	else
	{
		first_mutex = &conf->dongles[second_idx];
		second_mutex = &conf->dongles[first_idx];
	}
	usleep(100 * (conf->id % 2 == 0));
	conf->request_time = log_message(0, -1);
	while (!coder_iteration(conf, first_mutex, second_mutex,
			(first_idx == second_idx)))
		continue ;
	return (0);
}
