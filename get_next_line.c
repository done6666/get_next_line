/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opektas <opektas@student.42kocaeli.com.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 23:56:45 by opektas           #+#    #+#             */
/*   Updated: 2026/05/11 01:27:01 by opektas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static int	free_memory(t_memory **memory)
{
	if (!memory || !*memory)
		return (0);
	free((*memory)->backup);
	free((*memory)->buffer);
	free(*memory);
	*memory = NULL;
	return (0);
}

static int	read_to_newline(int fd, t_memory **memory)
{
	while ((*memory)->bytes && !ft_strchr((*memory)->backup, '\n'))
	{
		(*memory)->bytes = read(fd, (*memory)->buffer, BUFFER_SIZE);
		if ((*memory)->bytes == -1)
			return (free_memory(&(*memory)));
		(*memory)->buffer[(*memory)->bytes] = '\0';
		(*memory)->backup = ft_strjoin((*memory)->backup, (*memory)->buffer);
	}
	return (1);
}

static int	init_memory(t_memory **memory)
{
	if (*memory)
		return (1);
	*memory = malloc(sizeof(t_memory));
	if (!*memory)
		return (0);
	(*memory)->backup = NULL;
	(*memory)->newline = NULL;
	(*memory)->buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!(*memory)->buffer)
		return (free_memory(&(*memory)));
	(*memory)->bytes = 1;
	return (1);
}

static char	*cut_line(t_memory **memory)
{
	char	*line;

	if (!(*memory)->backup || !(*memory)->backup[0])
	{
		free_memory(memory);
		return (NULL);
	}
	(*memory)->newline = ft_strchr((*memory)->backup, '\n');
	if ((*memory)->newline)
		line = ft_substr((*memory)->backup, 0, (*memory)->newline
				- (*memory)->backup + 1);
	else
		line = ft_substr((*memory)->backup, 0, ft_strlen((*memory)->backup));
	if (!line)
	{
		free_memory(memory);
		return (NULL);
	}
	if ((*memory)->newline)
		(*memory)->newline = ft_strdup((*memory)->newline + 1);
	else
		(*memory)->newline = ft_strdup("");
	free((*memory)->backup);
	(*memory)->backup = (*memory)->newline;
	return (line);
}

char	*get_next_line(int fd)
{
	static t_memory	*memory;

	if (fd < 0 || BUFFER_SIZE <= 0 || !init_memory(&memory)
		||!read_to_newline(fd, &memory))
		return (NULL);
	return (cut_line(&memory));
}
