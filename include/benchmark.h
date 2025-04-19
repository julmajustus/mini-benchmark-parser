/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   benchmark.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmakkone <jmakkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 10:33:50 by jmakkone          #+#    #+#             */
/*   Updated: 2025/04/18 23:58:22 by jmakkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "test_entry.h"
#include <stdio.h>

typedef struct s_benchmark {
	t_test_entry         *data;
	char                *date;
	char                *kernel_ver;
	char                *system_info;
	int                 mode;
	struct s_benchmark  *next;
} t_benchmark;

t_benchmark *new_benchmark(t_test_entry *data, char *date, char *kernel_ver, char *system_info, int mode);
t_benchmark *last_benchmark(t_benchmark *lst);
void benchmark_add_back(t_benchmark **lst, t_benchmark *new_benchmark);
t_benchmark *find_benchmark_by_kernel(t_benchmark *lst, const char *kernel_ver, int mode);
t_benchmark *combine_benchmarks(const t_benchmark *benchmark);
t_benchmark *sort_benchmarks(t_benchmark *benchmark);
int get_benchmark_list_size(const t_benchmark *lst);
void clean_benchmarks(t_benchmark *lst);

#endif
