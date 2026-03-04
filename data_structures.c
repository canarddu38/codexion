/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   data_structures.c								  :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: julcleme <julcleme@student.42lyon.fr>	  +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2026/02/06 17:56:10 by julcleme		  #+#	#+#			 */
/*   Updated: 2026/02/10 11:46:22 by julcleme		 ###   ########lyon.fr   */
/*																			*/
/* ************************************************************************** */

#include "data_structures.h"

void	free_lst(t_lst *lst)
{
	t_lst	*current;
	t_lst	*next;

	current = lst;
	while (current)
	{
		next = current->next;
		free(current);
		current = next;
	}
}

t_lst	*create_lst(int nb, t_coder_config *coders)
{
	int		i;
	t_lst	*output;
	t_lst	*current;
	t_lst	*last_node;

	i = 0;
	output = 0;
	while (i < nb)
	{
		current = malloc(sizeof(*current));
		if (!current)
		{
			free_lst(output);
			return (0);
		}
		current->next = 0;
		current->coder = &(coders[i]);
		if (!output)
			output = current;
		else
			last_node->next = current;
		last_node = current;
		i++;
	}
	return (output);
}

static void	queue_edf_sort(t_lst **queue, int swap)
{
	t_lst			*i;
	t_coder_config	*tmp;
	int				deadlines[3];

	while (swap)
	{
		swap = 0;
		i = *queue;
		while (i && i->next)
		{
			deadlines[0] = get_deadline_edf(i->coder);
			deadlines[1] = get_deadline_edf(i->next->coder);
			if (deadlines[1] < deadlines[0] || (deadlines[1] == deadlines[0]
					&& ((i->next->coder->id % 2 > i->coder->id % 2)
						|| (i->next->coder->id % 2 == i->coder->id % 2
							&& i->next->coder->id < i->coder->id))))
			{
				tmp = i->coder;
				i->coder = i->next->coder;
				i->next->coder = tmp;
				swap = 1;
			}
			i = i->next;
		}
	}
}

static void	queue_fifo_sort(t_lst **queue, t_coder_config	*tmp)
{
	t_lst			*current;
	t_lst			*min;
	t_lst			*iter;

	if (!queue || !*queue || !(*queue)->next)
		return ;
	current = *queue;
	while (current)
	{
		min = current;
		iter = current->next;
		while (iter)
		{
			if (iter->coder->request_time < min->coder->request_time)
				min = iter;
			iter = iter->next;
		}
		if (min != current)
		{
			tmp = current->coder;
			current->coder = min->coder;
			min->coder = tmp;
		}
		current = current->next;
	}
}

void	queue_circle_next(t_lst **queue, t_scheduler_type type)
{
	if (!queue || !*queue || !(*queue)->next)
		return ;
	if (type == SCHEDULER_FIFO)
		queue_fifo_sort(queue, 0);
	else if (type == SCHEDULER_EDF)
		queue_edf_sort(queue, 1);
}
