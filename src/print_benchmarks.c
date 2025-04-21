/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_benchmarks.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmakkone <jmakkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 10:48:26 by jmakkone          #+#    #+#             */
/*   Updated: 2025/04/22 01:46:46 by jmakkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "print_benchmarks.h"

void print_benchmark(t_benchmark *benchmark, int print_sys_info)
{
	if (!benchmark)
		return;

	t_benchmark *bm;
	t_test_entry *te;

	int padding_width = 0;
	bm = benchmark;
	while (bm) {
		te = bm->data;
		while (te) {
			int len = strlen(te->name) + 1;
			if (len > padding_width) {
				padding_width = len;
			}
			te = te->next;
		}
		bm = bm->next;
	}

	bm = benchmark;
	while (bm) {
		printf("----------------------------\n");
		te = bm->data;
		printf("Kernel version: %s\n", bm->kernel_ver);
		if (bm->mode == 1) {
			printf("Mode: mini\n");
		}
		if (bm->mode == 2) {
			printf("Mode: nano\n");
		}
		printf("Date: %s\n", bm->date);
		printf("----------------------------\n");
		while (te) {
			printf("%-*s: %f\n", padding_width, te->name, te->result);
			te = te->next;
		}
		if (print_sys_info) {
			printf("----------------------------\n\n");
			printf("-----System information-----\n%s", bm->system_info);
		}
		bm = bm->next;
		printf("----------------------------\n\n");
	}
}

void print_kernel_comparison(t_benchmark *combined)
{
	if (!combined)
		return;

	int padding_width = 0, max_test_count = 0;
	t_benchmark *bm = combined;

	while (bm) {
		int len = strlen(bm->kernel_ver);
		int bm_test_count = get_test_entry_list_size(bm->data);
		if (len > padding_width) {
			padding_width = len;
		}
		max_test_count += bm_test_count;
		bm = bm->next;
	}

	int t_time = 0, t_score = 0, test_count = 0;
	char **test_names = malloc(sizeof(char *) * max_test_count + 2);
	if (!test_names) {
		fprintf(stderr, "Memory allocation error.\n");
		return;
	}

	bm = combined;
	while (bm) {
		t_test_entry *te = bm->data;
		while (te) {
			int is_dublicate = 0;
			for (int i = 0; i < test_count; i++) {
				if (strcmp(test_names[i], te->name) == 0) {
					is_dublicate = 1;
					break;
				}
			}
			if (!is_dublicate) {
				if  (strcmp(te->name, "Total time (s)") == 0)
					t_time = 1;
				else if  (strcmp(te->name, "Total score") == 0)
					t_score = 1;
				else
					test_names[test_count++] = te->name;
			}
			te = te->next;
		}
		bm = bm->next;
	}

	if (t_time)
		test_names[test_count++] = "Total time (s)";
	if (t_score)
		test_names[test_count++] = "Total score";

	for (int i = 0; i < test_count; i++) {
		printf("%s\n", test_names[i]);
		printf("----------------------------\n");
		bm = combined;
		while (bm) {
			t_test_entry *entry = find_test_entry(bm->data, test_names[i]);
			if (entry) {
				if (bm->mode) {
					if (bm->mode == 1)
						printf("%-*s mini: %f\n", padding_width, bm->kernel_ver, entry->result);
					else if (bm->mode == 2)
						printf("%-*s nano: %f\n", padding_width, bm->kernel_ver, entry->result);
				}
				else
				printf("%-*s : %f\n", padding_width, bm->kernel_ver, entry->result);
			}
			bm = bm->next;
		}
		printf("----------------------------\n\n");
	}
	free(test_names);
}
