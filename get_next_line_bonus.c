/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fernfern <fernfern@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/03 23:12:52 by fernfern          #+#    #+#             */
/*   Updated: 2026/10/03 23:37:24 by fernfern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static char	*ft_grow_join(char *buf, char *tmp, size_t *cap, size_t len)
{
	char	*new_buf;
	size_t	i;

	if (len + BUFFER_SIZE + 1 > *cap)
	{
		*cap = (*cap * 2);
		if (*cap < len + BUFFER_SIZE + 1)
			*cap = len + BUFFER_SIZE + 1024;
		new_buf = malloc(*cap);
		if (!new_buf)
			return (free(buf), NULL);
		i = -1;
		while (++i < len)
			new_buf[i] = buf[i];
		free(buf);
		buf = new_buf;
	}
	i = 0;
	while (tmp[i])
		buf[len++] = tmp[i++];
	buf[len] = '\0';
	return (buf);
}

static char	*ft_reading(int fd, char *buffer)
{
	char	*tmp;
	size_t	cap;
	size_t	len;
	ssize_t	r;

	if (ft_strchr(buffer, '\n'))
		return (buffer);
	tmp = malloc(BUFFER_SIZE + 1);
	if (!tmp)
		return (ft_free_all(&buffer, NULL));
	len = ft_strlen(buffer);
	cap = len;
	r = 1;
	while (r > 0)
	{
		r = read(fd, tmp, BUFFER_SIZE);
		if (r < 0)
			return (ft_free_all(&buffer, &tmp));
		tmp[r] = '\0';
		buffer = ft_grow_join(buffer, tmp, &cap, len);
		if (!buffer || ft_strchr(tmp, '\n') || r == 0)
			break ;
		len += r;
	}
	return (ft_free_all(&tmp, NULL), buffer);
}

static char	*ft_nl(char **buffer)
{
	char	*line;
	char	*rem;
	size_t	i;
	size_t	total_len;

	if (!buffer || !*buffer || **buffer == '\0')
		return (ft_free_all(buffer, NULL));
	i = 0;
	while ((*buffer)[i] && (*buffer)[i] != '\n')
		i++;
	if ((*buffer)[i] == '\n')
		i++;
	line = ft_substr(*buffer, 0, i);
	if (!line)
		return (ft_free_all(buffer, NULL));
	total_len = ft_strlen(*buffer);
	rem = ft_substr(*buffer, i, total_len - i);
	free(*buffer);
	*buffer = rem;
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*buffers[MAX_FD];
	char		*line;

	if (fd < 0 || fd >= MAX_FD || BUFFER_SIZE <= 0)
		return (NULL);
	buffers[fd] = ft_reading(fd, buffers[fd]);
	if (!buffers[fd])
		return (NULL);
	line = ft_nl(&buffers[fd]);
	if (!line)
		ft_free_all(&buffers[fd], NULL);
	return (line);
}
