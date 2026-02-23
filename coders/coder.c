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
	int	last;

	pthread_mutex_lock(&coder->time_mutex);
	last = coder->last_time_compiled;
	pthread_mutex_unlock(&coder->time_mutex);
	return (last + coder->program_args.time_to_burnout);
}

static int	coder_compile(t_coder_config *conf,
	pthread_mutex_t *first_mutex,
	pthread_mutex_t *second_mutex, bool one_coder)
{
	scheduler_wait(conf);
	pthread_mutex_lock(&conf->scheduler_mutex->mutex);
	if (conf->scheduler_mutex->stop_simulation)
	{
		pthread_mutex_unlock(&conf->scheduler_mutex->mutex);
		return (1);
	}
	pthread_mutex_unlock(&conf->scheduler_mutex->mutex);
	pthread_mutex_lock(first_mutex);
	log_message("has taken a dongle", conf->id);
	while (one_coder)
	{
		if (conf->scheduler_mutex->stop_simulation)
			return (1);
	}
	pthread_mutex_lock(second_mutex);
	log_message("has taken a dongle", conf->id);
	log_message("is compiling", conf->id);
	usleep(conf->program_args.time_to_compile * 1000);
	pthread_mutex_unlock(first_mutex);
	pthread_mutex_unlock(second_mutex);
	scheduler_signal(conf);
	return (0);
}

static int	coder_iteration(t_coder_config *conf,
	pthread_mutex_t *first_mutex,
	pthread_mutex_t *second_mutex, bool one_coder)
{
	pthread_mutex_lock(&conf->scheduler_mutex->mutex);
	if (conf->scheduler_mutex->stop_simulation)
		return (1 + 0 * pthread_mutex_unlock(&conf->scheduler_mutex->mutex));
	pthread_mutex_unlock(&conf->scheduler_mutex->mutex);
	if (conf->state == COMPILE)
		return (coder_compile(conf, first_mutex, second_mutex, one_coder));
	else if (conf->state == DEBUG)
	{
		log_message("is debugging", conf->id);
		usleep(conf->program_args.time_to_debug * 1000);
		pthread_mutex_lock(&conf->scheduler_mutex->mutex);
		conf->state = REFACTOR;
	}
	else if (conf->state == REFACTOR)
	{
		log_message("is refactoring", conf->id);
		usleep(conf->program_args.time_to_refactor * 1000);
		pthread_mutex_lock(&conf->scheduler_mutex->mutex);
		conf->request_time = log_message(0, -1);
		conf->state = COMPILE;
	}
	pthread_mutex_unlock(&conf->scheduler_mutex->mutex);
	return (0);
}

void	*coder_thread(void	*arg)
{
	int				idx[2];
	t_coder_config	*conf;
	pthread_mutex_t	*first_mutex;
	pthread_mutex_t	*second_mutex;

	conf = (t_coder_config *)arg;
	idx[0] = conf->id - 1;
	idx[1] = conf->id % conf->program_args.nb_coders;
	if (idx[0] < idx[1])
	{
		first_mutex = &conf->dongles[idx[0]];
		second_mutex = &conf->dongles[idx[1]];
	}
	else
	{
		first_mutex = &conf->dongles[idx[1]];
		second_mutex = &conf->dongles[idx[0]];
	}
	usleep(100 * (conf->id % 2 == 0));
	pthread_mutex_lock(&conf->scheduler_mutex->mutex);
	conf->request_time = log_message(0, -1);
	pthread_mutex_unlock(&conf->scheduler_mutex->mutex);
	while (!coder_iteration(conf, first_mutex, second_mutex, \
(idx[0] == idx[1])))
		continue ;
	return (0);
}
