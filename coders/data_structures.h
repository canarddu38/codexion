/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data_structures.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 17:56:10 by julcleme          #+#    #+#             */
/*   Updated: 2026/02/10 11:46:22 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATA_STRUCTURES_H
# define DATA_STRUCTURES_H
# include <sys/time.h>
# include <pthread.h>
# include <stdbool.h>
# include <stdlib.h>

typedef enum t_scheduler_type
{
	SCHEDULER_FIFO,
	SCHEDULER_EDF
}	t_scheduler_type;

typedef struct s_lst
{
	struct s_lst			*next;
	struct s_coder_config	*coder;
}	t_lst;

typedef struct s_scheduler
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				stop_simulation;
	int				*dongle_state;
	size_t			*dongle_release;
	t_lst			*queue;
}	t_scheduler;

typedef struct s_args
{
	int					nb_coders;
	int					time_to_burnout;
	int					time_to_compile;
	int					time_to_debug;
	int					time_to_refactor;
	int					nb_compiles_required;
	int					dongle_cooldown;
	t_scheduler_type	scheduler;
}	t_args;

typedef enum t_coder_state
{
	COMPILE,
	DEBUG,
	REFACTOR
}	t_coder_state;

typedef struct s_coder_config
{
	int						compiled;
	int						last_time_compiled;
	int						id;
	pthread_t				thread;
	t_args					program_args;
	t_coder_state			state;
	pthread_mutex_t			*dongles;
	pthread_cond_t			cond;
	t_scheduler				*scheduler_mutex;
	int						request_time;
	bool					has_lock;
}	t_coder_config;

void	*coder_thread(void	*arg);
int		log_message(char *msg, int id);

void	free_lst(t_lst *lst);
t_lst	*create_lst(int nb, t_coder_config *coders);
void	queue_circle_next(t_lst **queue, t_scheduler_type type);
int		get_deadline_edf(t_coder_config *coder);

#endif