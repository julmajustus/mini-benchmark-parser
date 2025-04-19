/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_entry.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmakkone <jmakkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 10:37:39 by jmakkone          #+#    #+#             */
/*   Updated: 2025/04/18 23:59:10 by jmakkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "test_entry.h"

t_test_entry *new_test_entry(char *name, double result)
{
	t_test_entry *new_node;

	new_node = malloc(sizeof(t_test_entry));
	if (!new_node)
		return (NULL);
	new_node->name = name;
	new_node->result = result;
	new_node->sum = result;
	new_node->count = 1;
	new_node->next = NULL;
	return (new_node);
}

t_test_entry *last_test_entry(t_test_entry *lst)
{
	t_test_entry *lptr;

	if (!lst)
		return (NULL);
	lptr = lst;
	while (lptr->next)
		lptr = lptr->next;
	return (lptr);
}

void test_entry_add_back(t_test_entry **lst, t_test_entry *new)
{
	t_test_entry *ptr;

	if (!new)
		return ;
	if (!*lst) {
		*lst = new;
		return ;
	}
	ptr = last_test_entry(*lst);
	ptr->next = new;
}

t_test_entry *find_test_entry(t_test_entry *lst, const char *name)
{
    while (lst) {
        if (strcmp(lst->name, name) == 0)
            return lst;
        lst = lst->next;
    }
    return NULL;
}

int get_test_entry_list_size(const t_test_entry *lst)
{
	int len = 0;
	while (lst) {
		len++;
		lst = lst->next;
	}
	return len;
}

void merge_test_entry(t_test_entry **head, t_test_entry *new_entry)
{
	t_test_entry *existing = find_test_entry(*head, new_entry->name);
	if (existing) {
		existing->sum += new_entry->sum;
		existing->count += new_entry->count;
		existing->result = existing->sum / existing->count;
		free(new_entry->name);
		free(new_entry);
	}
	else {
		if (!*head)
			*head = new_entry;
		else {
			t_test_entry *tmp = *head;
			while (tmp->next)
				tmp = tmp->next;
			tmp->next = new_entry;
		}
	}
}

void clean_test_entrys(t_test_entry *entry)
{
	t_test_entry *ptr;

	while (entry) {
		free(entry->name);
		ptr = entry->next;
		free(entry);
		entry = ptr;
	}
}
