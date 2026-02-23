/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julcleme <julcleme@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 01:56:14 by julcleme          #+#    #+#             */
/*   Updated: 2026/02/22 15:58:24 by julcleme         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODER_UTILS_H
# define CODER_UTILS_H

# include <stdbool.h>
# include <stdio.h>
# include <string.h>
# include "data_structures.h"

bool	can_compile(t_coder_config *me);
void	scheduler_wait(t_coder_config *conf);
void	scheduler_signal(t_coder_config *conf);
bool	is_coder_busy(t_coder_config *me,
			int first_dongle, int second_dongle, size_t current_time);

#endif