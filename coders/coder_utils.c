/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 01:55:00 by julcleme          #+#    #+#             */
/*   Updated: 2026/02/23 20:50:57 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "coder_utils.h"

int	log_message(char *msg, int id)
{
	size_t					curr_time;
	static size_t			start_time;
	static pthread_mutex_t	log_mutex = PTHREAD_MUTEX_INITIALIZER;
	static bool				finished;
	struct timeval			vt;

	pthread_mutex_lock(&log_mutex);
	gettimeofday(&vt, 0);
	if (start_time == 0)
		start_time = vt.tv_sec * 1000 + vt.tv_usec / 1000;
	curr_time = vt.tv_sec * 1000 + vt.tv_usec / 1000;
	if (id > -1 && msg && !finished)
		printf("%lu %i %s\n", (curr_time - start_time), id, msg);
	if (msg && !strcmp(msg, "burned out"))
		finished = true;
	pthread_mutex_unlock(&log_mutex);
	return (curr_time - start_time);
}

bool	can_compile(t_coder_config *me)
{
	int				deadline;
	int				first_dongle;
	int				second_dongle;
	size_t			current_time;
	t_lst			*current;

	first_dongle = me->id - 1;
	second_dongle = me->id % me->program_args.nb_coders;
	current_time = log_message(0, -1);
	if (is_coder_busy(me, first_dongle, second_dongle, current_time))
		return (false);
	if (me->program_args.scheduler == SCHEDULER_EDF)
	{
		current = me->scheduler_mutex->queue;
		deadline = get_deadline_edf(current->coder)
			+ me->program_args.time_to_compile;
		while (current && get_deadline_edf(current->coder) <= deadline)
		{
			if (me->id == current->coder->id)
				return (true);
			current = current->next;
		}
		return (false);
	}
	return (true);
}

void	scheduler_wait(t_coder_config *conf)
{
	pthread_mutex_lock(&conf->scheduler_mutex->mutex);
	while (!can_compile(conf) && !conf->scheduler_mutex->stop_simulation)
		pthread_cond_wait(&conf->scheduler_mutex->cond,
			&conf->scheduler_mutex->mutex);
	if (!conf->scheduler_mutex->stop_simulation)
	{
		conf->has_lock = true;
		conf->scheduler_mutex->dongle_state[conf->id - 1] = 1;
		conf->scheduler_mutex->dongle_state[conf->id
			% conf->program_args.nb_coders] = 1;
	}
	pthread_mutex_unlock(&conf->scheduler_mutex->mutex);
}

void	scheduler_signal(t_coder_config *conf)
{
	int	current_time;

	pthread_mutex_lock(&conf->scheduler_mutex->mutex);
	conf->has_lock = false;
	current_time = log_message(0, -1);
	conf->scheduler_mutex->dongle_release[conf->id - 1] = \
current_time + conf->program_args.dongle_cooldown;
	conf->scheduler_mutex->dongle_release[conf->id
		% conf->program_args.nb_coders] = \
current_time + conf->program_args.dongle_cooldown;
	conf->scheduler_mutex->dongle_state[conf->id - 1] = 0;
	conf->scheduler_mutex->dongle_state[conf->id
		% conf->program_args.nb_coders] = 0;
	conf->state = DEBUG;
	pthread_mutex_lock(&conf->time_mutex);
	conf->last_time_compiled = current_time
		+ conf->program_args.time_to_compile;
	conf->compiled++;
	pthread_mutex_unlock(&conf->time_mutex);
	if (conf->program_args.nb_coders > 1)
		queue_circle_next(&(conf->scheduler_mutex->queue),
			conf->program_args.scheduler);
	pthread_cond_broadcast(&conf->scheduler_mutex->cond);
	pthread_mutex_unlock(&conf->scheduler_mutex->mutex);
}

bool	is_coder_busy(t_coder_config *me,
	int first_dongle, int second_dongle, size_t current_time)
{
	t_lst	*current;
	int		coder_dongles[2];

	if (me->program_args.scheduler == SCHEDULER_FIFO)
	{
		current = me->scheduler_mutex->queue;
		while (current)
		{
			coder_dongles[0] = current->coder->id
				% current->coder->program_args.nb_coders;
			coder_dongles[1] = current->coder->id - 1;
			if (current->coder->state == COMPILE
				&& (coder_dongles[0] == first_dongle
					|| coder_dongles[1] == first_dongle
					|| coder_dongles[0] == second_dongle
					|| coder_dongles[1] == second_dongle)
				&& current->coder->request_time < me->request_time)
				return (true);
			current = current->next;
		}
	}
	return (me->scheduler_mutex->dongle_state[first_dongle]
		|| me->scheduler_mutex->dongle_state[second_dongle]
		|| current_time < me->scheduler_mutex->dongle_release[first_dongle]
		|| current_time < me->scheduler_mutex->dongle_release[second_dongle]);
}
