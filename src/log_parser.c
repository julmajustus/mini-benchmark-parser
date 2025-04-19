/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmakkone <jmakkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 10:51:05 by jmakkone          #+#    #+#             */
/*   Updated: 2025/04/19 00:01:17 by jmakkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "log_parser.h"

static int is_on_filterlist(const char *test, const char *filterlist)
{
	char *token;
	
	char *filters = strdup(filterlist);
	if (!filters)
		return 0;

	if (!(strchr(filters, '|'))) {
		int ret = strcmp(test, filters) == 0;
		free(filters);
		return ret;
	}
	
	token = strtok(filters, "|");
	while (token) {
		if (strcmp(test, token) == 0) {
			free(filters);
			return 1;
		}
		token = strtok(NULL, "|");
	}
	free(filters);
	return 0;
}

t_benchmark *read_logs(const char *path, const char *kernel_filter, const char *test_filter,
					   const char *exclude_kernel_filter, const char *exclude_test_filter)
{
	
	t_benchmark *bm = NULL;
	struct dirent *entry;

	DIR *dir = opendir(path);
	if (!dir) {
		fprintf(stderr, "Failed to open directory: %s\n", path);
		return NULL;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (strncmp(entry->d_name, "benchie_", 8) == 0) {
		
			int path_len = strlen(path) + strlen(entry->d_name) + 1;
			char *full_path = calloc(path_len, path_len);
			if (!full_path) {
				fprintf(stderr, "Calloc failed\n");
				continue;
			}
			full_path = strcpy(full_path, path);
			if (full_path[strlen(full_path)] != '/')
				full_path = strcat(full_path, "/");
			full_path = strcat(full_path, entry->d_name);
			if (!full_path) {
				fprintf(stderr, "Failed to create full path from: %s +  %s\n",path, entry->d_name);
				continue;
			}
			FILE *f = fopen(full_path, "r");
			
			if (!f) {
				fprintf(stderr, "Failed to open file: %s\n", entry->d_name);
				continue;
			}
			free(full_path);

			t_test_entry *te = NULL;
			
			char line[BUF_SIZE];
			memset(line, '\0', BUF_SIZE);
			char *date = NULL;
			char *kernel_ver = NULL;
			char *system_info = NULL;
			int mode = 0;
			int test_data_collected = 0;
			int is_malformed = 0;
			while (fgets(line, sizeof(line), f)) {
				if (test_data_collected) {
					int new_len = strlen(system_info) + strlen(line) + 1;
					char *new_system_info = realloc(system_info, new_len);
					if (!new_system_info) {
						fprintf(stderr, "Calloc failed when collecting system information\n");
						is_malformed = 1;
						break;
					}
					system_info = new_system_info;
					strcat(system_info, line);
				}
				else if (strspn(line, " \t\n\v\f\r") == strlen(line))
					continue;
				else if (strncmp(line, "Mode: mini", 10) == 0) {
					mode = 1;
					continue;
				}
				else if (strncmp(line, "Mode: nano", 10) == 0) {
					mode = 2;
					continue;
				}
				else if (strncmp(line, "Date: ", 6) == 0)
					date = strndup(line + 6, (strlen(line) - 7));
				else if (strncmp(line, "System:    Kernel: ", 19) == 0) {
					const char *p = line + 19;
					const char *end = strchr(p, ' ');
					kernel_ver = strndup(p, end - p);
					system_info = strdup(line);
					test_data_collected = 1;
					continue;
				}
				else {
					char *test_name = NULL;
					char *test_result_str = NULL;
					double test_result = 0;
					
					test_name = strndup(line, (int)(strchr(line, ':') - line));
					if (!test_name) {
						fprintf(stderr, "Failed to read test name from: %s\n", line);
						continue;
					}
					
					if (test_filter) {
						if (!is_on_filterlist(test_name, test_filter)) {
							free(test_name);
							continue;
						}
					}
					if (exclude_test_filter) {
						if (is_on_filterlist(test_name, exclude_test_filter)) {
							free(test_name);
							continue;
						}
					}

					test_result_str = strndup(line + strlen(test_name) + 2, strlen(line));
					if (!test_result_str) {
						fprintf(stderr, "Failed to read result from: %s\n", line);
						continue;
					}

					test_result = atof(test_result_str);
					free(test_result_str);

					if (!te)
						te = new_test_entry(test_name, test_result);
					else
						test_entry_add_back(&te, new_test_entry(test_name, test_result));
				}
			}
			
			if (!date || !kernel_ver)
				is_malformed = 1;
			
			if (kernel_filter && kernel_ver) {
				if (!is_on_filterlist(kernel_ver, kernel_filter)) {
					free(kernel_ver);
					free(date);
					free(system_info);
					clean_test_entrys(te);
					if (fclose(f) != 0) {
						fprintf(stderr, "Failed to close file\n");
						clean_benchmarks(bm);
						closedir(dir);
						return NULL;
					}
					continue;
				}
			}

			if (exclude_kernel_filter && kernel_ver) {
				if (is_on_filterlist(kernel_ver, exclude_kernel_filter)) {
					free(kernel_ver);
					free(date);
					free(system_info);
					clean_test_entrys(te);
					if (fclose(f) != 0) {
						fprintf(stderr, "Failed to close file\n");
						clean_benchmarks(bm);
						closedir(dir);
						return NULL;
					}
					continue;
				}
			}

			if (!te) {
				free(kernel_ver);
				free(date);
				free(system_info);
				if (fclose(f) != 0) {
					fprintf(stderr, "Failed to close file\n");
					clean_benchmarks(bm);
					closedir(dir);
					return NULL;
				}
				continue;
			}

			if (!bm)
				bm = new_benchmark(te, date, kernel_ver, system_info, mode);
			else
				benchmark_add_back(&bm, new_benchmark(te, date, kernel_ver, system_info, mode));

			if (is_malformed) {
				fprintf(stderr, "Malformed log file: %s\n", entry->d_name);
				if (fclose(f) != 0)
					fprintf(stderr, "Failed to close file\n");
				closedir(dir);
				clean_benchmarks(bm);
				return NULL;
			}
			else if (fclose(f) != 0) {
				fprintf(stderr, "Failed to close file\n");
				clean_benchmarks(bm);
				closedir(dir);
				return NULL;
			}
		}
	}
	closedir(dir);
	return bm;
}
