/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokens_str_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/21 19:43:15 by dmonteir          #+#    #+#             */
/*   Updated: 2022/03/31 21:47:39 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "minishell.h"

char	*worddup(char **s)
{
	char	*str;
	size_t	offset;
	size_t	len;
	char	*temp;
	//char	*join;

	if (*s == NULL)
		return (NULL);
	temp = *s;
	len = 0;
	str = NULL;
	//join = ft_strdup("");

	/* while (!ft_strchr("| ", *temp) && *temp)
	{
		if (*temp == QUOT || *temp == DQUOT)
			temp++;
		else
		{
			str = join;
			join = ft_catchr(join, *temp);
			free(str);
			temp++;
		}
		len++;
	} */

	while (!ft_strchr("| ", *temp) && *temp)
	{
		len++;
		temp++;
	}
	str = malloc(len + 1);
	offset = 0;
	while (offset < len)
	{
		str[offset] = (*s)[offset];
		offset++;
	}
	str[offset] = '\0';
	*s += len;
	return (str);
}

//static char	*ft_catchr(char *str, char c)
//{
//	char	*str_cat;
//	size_t	len;
//	int		i;
//
//	i = 0;
//	len = ft_strlen(str);
//	str_cat = malloc(sizeof(char) * (len + 2));
//	while(str[i])
//	{
//		str_cat[i] = str[i];
//		i++;
//	}
//	str_cat[i] = c;
//	str_cat[i + 1] = '\0';
//
//	return (str_cat);
//}

// copiar string até o final, não só até as próximas aspas, ex: "c"at
char	*quotdup(char **s, char	quot)
{
	char	*str;
	size_t	offset;
	size_t	len;

	if (*s == NULL)
		return (NULL);
	str = *s;
	len = 0;
	len++;
	while (str[len] != quot)
		len++;
	str = malloc(len + 2);
	offset = 0;
	while (offset <= len)
	{
		str[offset] = (*s)[offset];
		offset++;
	}
	str[offset] = '\0';
	*s += len + 1;
	return (str);
}

int	has_double_quotation(char *str, char quot)
{
	int	ret;
	int	count;

	ret = 0;
	count = 0;
	while (*str)
	{
		if (*str == quot)
			count++;
		str++;
	}
	if (count % 2 != 0)
		ret = 1;
	return (ret);
}
