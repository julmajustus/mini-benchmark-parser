/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   benchmark.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmakkone <jmakkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 10:42:47 by jmakkone          #+#    #+#             */
/*   Updated: 2025/04/18 23:58:10 by jmakkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "benchmark.h"

t_benchmark *new_benchmark(t_test_entry *data, char *date, char *kernel_ver, char *system_info, int mode)
{
	t_benchmark	*new_node;

	new_node = malloc(sizeof(t_benchmark));
	if (!new_node)
		return (NULL);
	new_node->data = data;
	new_node->date = date;
	new_node->kernel_ver = kernel_ver;
	new_node->system_info = system_info;
	new_node->mode = mode;
	new_node->next = NULL;
	return (new_node);
}

t_benchmark *last_benchmark(t_benchmark *lst)
{
	t_benchmark	*lptr;

	if (!lst)
		return (NULL);
	lptr = lst;
	while (lptr->next)
		lptr = lptr->next;
	return (lptr);
}

void benchmark_add_back(t_benchmark **lst, t_benchmark *new)
{
	t_benchmark	*ptr;

	if (!new)
		return ;
	if (!*lst) {
		*lst = new;
		return ;
	}
	ptr = last_benchmark(*lst);
	ptr->next = new;
}

t_benchmark *find_benchmark_by_kernel(t_benchmark *lst, const char *kernel_ver, int mode)
{
	while (lst) {
		if (strcmp(lst->kernel_ver, kernel_ver) == 0 && lst->mode == mode)
			return lst;
		lst = lst->next;
	}
	return NULL;
}

t_benchmark *combine_benchmarks(const t_benchmark *benchmark)
{
	t_benchmark *combined = NULL;

	while (benchmark) {
		t_benchmark *found = find_benchmark_by_kernel(combined, benchmark->kernel_ver, benchmark->mode);
		if (!found) {
			t_test_entry *dup_list = NULL;
			t_test_entry *temp = benchmark->data;
			while (temp) {
				test_entry_add_back(&dup_list, new_test_entry(strdup(temp->name), temp->result));
				temp = temp->next;
			}
			benchmark_add_back(&combined,
					  new_benchmark(dup_list, strdup(benchmark->date), strdup(benchmark->kernel_ver), strdup(benchmark->system_info), benchmark->mode));
		}
		else {
			t_test_entry *temp = benchmark->data;
			while (temp) {
				merge_test_entry(&(found->data), new_test_entry(strdup(temp->name), temp->result));
				temp = temp->next;
			}
		}
		benchmark = benchmark->next;
	}
	return combined;
}

static int compare_kernel_ver(const t_benchmark *a, const t_benchmark *b)
{
	int a_major = 0, a_minor = 0, a_patch = 0;
	int b_major = 0, b_minor = 0, b_patch = 0;
	size_t a_len, b_len;
	char *a_num = NULL, *b_num = NULL;

	a_len = strcspn(a->kernel_ver, "-");
	b_len = strcspn(b->kernel_ver, "-");

	a_num = strndup(a->kernel_ver, a_len);
	b_num = strndup(b->kernel_ver, b_len);
	if (!a_num || !b_num) {
		free(a_num);
		free(b_num);
		return strcmp(a->kernel_ver, b->kernel_ver);
	}

	if (sscanf(a_num, "%d.%d.%d", &a_major, &a_minor, &a_patch) != 3) {
		free(a_num);
		free(b_num);
		return strcmp(a->kernel_ver, b->kernel_ver);
	}
	if (sscanf(b_num, "%d.%d.%d", &b_major, &b_minor, &b_patch) != 3) {
		free(a_num);
		free(b_num);
		return strcmp(a->kernel_ver, b->kernel_ver);
	}

	free(a_num);
	free(b_num);

	if (a_major != b_major)
		return a_major - b_major;
	if (a_minor != b_minor)
		return a_minor - b_minor;
	return a_patch - b_patch;
}

static int compare_benchmark_ptr(const void *pa, const void *pb)
{
    const t_benchmark *a = *(const t_benchmark **)pa;
    const t_benchmark *b = *(const t_benchmark **)pb;
    return compare_kernel_ver(a, b);
}

t_benchmark *sort_benchmarks(t_benchmark *benchmark)
{

    int count = get_benchmark_list_size(benchmark);
    if (count == 0)
        return NULL;

    t_benchmark **arr = malloc(count * sizeof(t_benchmark *));
    if (!arr) {
		fprintf(stderr, "Malloc failed at sort_benchmarks\n");
        return benchmark;
	}

    int i = 0;
    for (t_benchmark *cur = benchmark; cur != NULL; cur = cur->next) {
        arr[i++] = cur;
    }

    qsort(arr, count, sizeof(t_benchmark *), compare_benchmark_ptr);

    for (i = 0; i < count - 1; i++) {
        arr[i]->next = arr[i + 1];
    }
    arr[count - 1]->next = NULL;

    t_benchmark *sorted = arr[0];
    free(arr);
    return sorted;
}

int get_benchmark_list_size(const t_benchmark *lst)
{
	int len = 0;
	while (lst) {
		len++;
		lst = lst->next;
	}
	return len;
}

void clean_benchmarks(t_benchmark *lst)
{
	t_benchmark *ptr;
	if (!lst)
		return;

	while (lst) {
		clean_test_entrys(lst->data);
		if (lst->date)
			free(lst->date);
		if (lst->kernel_ver)
			free(lst->kernel_ver);
		if (lst->system_info)
			free(lst->system_info);
		ptr = lst->next;
		free(lst);
		lst = ptr;
	}
}
