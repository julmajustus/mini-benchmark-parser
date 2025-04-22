/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_entry.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmakkone <jmakkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 10:33:06 by jmakkone          #+#    #+#             */
/*   Updated: 2025/04/22 03:06:29 by jmakkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TEST_ENTRY_H
#define TEST_ENTRY_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct s_test_entry {
	char                *name;
	double              result;
	double              sum;
	int                 count;
	struct s_test_entry *next;
}	t_test_entry;

t_test_entry	*new_test_entry(char *name, double result);
t_test_entry	*last_test_entry(t_test_entry *lst);
void			test_entry_add_back(t_test_entry **lst, t_test_entry *new_entry);
t_test_entry	*find_test_entry(t_test_entry *lst, const char *name);
size_t			get_test_entry_list_size(const t_test_entry *lst);
void			merge_test_entry(t_test_entry **head, t_test_entry *new_entry);
void			clean_test_entries(t_test_entry *entry);

#endif
