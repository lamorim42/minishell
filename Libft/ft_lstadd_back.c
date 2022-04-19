/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lamorim <lamorim@student.42sp.org.br>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/07 13:31:24 by dmonteir          #+#    #+#             */
/*   Updated: 2022/04/18 20:55:53 by lamorim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *_new)
{
	t_list	*ptr;

	if (!_new)
		return ;
	if (!*lst)
	{
		*lst = _new;
		return ;
	}
	ptr = ft_lstlast(*lst);
	ptr->next = _new;
}
