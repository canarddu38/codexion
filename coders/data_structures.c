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

static void	queue_edf(t_lst **queue)
{
	t_lst			*current;
	t_lst			*best;
	t_coder_config	*tmp;

	best = *queue;
	current = (*queue)->next;
	while (current)
	{
		if (get_deadline_edf(current->coder)
			< get_deadline_edf(best->coder))
			best = current;
		current = current->next;
	}
	if (best != *queue)
	{
		tmp = (*queue)->coder;
		(*queue)->coder = best->coder;
		best->coder = tmp;
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
		queue_edf(queue);
}
