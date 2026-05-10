/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opektas <opektas@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 23:56:45 by opektas           #+#    #+#             */
/*   Updated: 2026/05/10 18:56:07 by opektas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static void	*free_memory(t_memory **memory)
{
	if (!*memory || !memory)
		return (NULL);
	free((*memory)->backup);
	free((*memory)->buffer);
	free(*memory);
	*memory = NULL;
	return (NULL);
}

static char	*read_to_newline(int fd, t_memory **memory)
{
	while ((*memory)->bytes && !ft_strchr((*memory)->backup, '\n'))
	{
		(*memory)->bytes = read(fd, (*memory)->buffer, BUFFER_SIZE);
		if ((*memory)->bytes == -1)
			return (free_memory(&(*memory)));
		(*memory)->buffer[(*memory)->bytes] = '\0';
		(*memory)->backup = ft_strjoin((*memory)->backup, (*memory)->buffer);
	}
	free((*memory)->buffer);
	return ((*memory)->backup);
}

static int	init_memory(t_memory **memory)
{
	if (*memory)
		return (1);
	*memory = malloc(sizeof(t_memory));
	if (!memory)
		return (0);
	(*memory)->backup = NULL;
	(*memory)->buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!(*memory)->buffer)
	{
		free_memory(&(*memory));
		return (0);
	}
	(*memory)->bytes = 1;
	return (1);
}

char	*get_next_line(int fd)
{
	static t_memory	*memory;

	if (fd < 0 || BUFFER_SIZE <= 0 || !init_memory(&memory))
		return (NULL);

	return (0);
}
