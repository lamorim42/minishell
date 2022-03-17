/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 15:30:55 by dmonteir          #+#    #+#             */
/*   Updated: 2021/06/08 23:40:15 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*ptr;
	t_list	*new_list;

	if (!lst)
		return (NULL);
	new_list = ft_lstnew(f(lst->content));
	if (!new_list)
	{
		ft_lstdelone(new_list, del);
		return (NULL);
	}
	ptr = new_list;
	lst = lst->next;
	while (lst != NULL)
	{
		new_list->next = ft_lstnew(f(lst->content));
		if (!(new_list->next))
		{
			ft_lstclear(&ptr, del);
			return (NULL);
		}
		lst = lst->next;
		new_list = new_list->next;
	}
	return (ptr);
}
