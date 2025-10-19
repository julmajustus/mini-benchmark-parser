/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmakkone <jmakkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 10:51:05 by jmakkone          #+#    #+#             */
/*   Updated: 2025/04/22 02:27:58 by jmakkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "log_parser.h"

static int parse_result(const char *str, double *result)
{
	char *endptr;
	errno = 0;
	double val = strtod(str, &endptr);

	if (endptr == str) {
		fprintf(stderr, "Parsing failed: invalid numeric (no digits)\n");
		return 0;
	}

	if (errno == ERANGE) {
		fprintf(stderr, "Parsing failed: numeric out of range\n");
		return 0;
	}

	while (isspace((unsigned char)*endptr))
		endptr++;
	if (*endptr != '\0') {
		fprintf(stderr, "Parsing failed: trailing characters after number\n");
		return 0;
	}
	
	*result = val;

	return 1;
}

static int is_on_filterlist(const char *test, const char *filterlist)
{
	if (!filterlist)
		return 0;

	if (!(strchr(filterlist, '|')))
		return strcmp(test, filterlist) == 0;

	char *filters = strdup(filterlist);
	if (!filters)
		return 0;

	char *saveptr = NULL;
	char *token = strtok_r(filters, "|", &saveptr);
	while (token) {
		if (strcmp(test, token) == 0) {
			free(filters);
			return 1;
		}
		token = strtok_r(NULL, "|", &saveptr);
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
			size_t n = strlen(path);
			size_t m = strlen(entry->d_name);
			size_t bufsize = n + 1 + m + 1;
			char *full_path = malloc(bufsize);
			if (!full_path) {
				fprintf(stderr, "Failed to create full path from: %s +  %s\n",path, entry->d_name);
				continue;
			}

			if (snprintf(full_path, bufsize, "%s%s%s",
				path,
				(path[n-1]=='/' ? "" : "/"),
				entry->d_name) >= (int)bufsize) {
				fprintf(stderr, "Failed to create fullpath: path too long\n");
				free(full_path);
				continue;
			}

			FILE *f = fopen(full_path, "r");
			free(full_path);
			if (!f) {
				fprintf(stderr, "Failed to open file: %s\n", entry->d_name);
				continue;
			}

			t_test_entry *te = NULL;
			char line[BUF_SIZE];
			memset(line, '\0', BUF_SIZE);
			char *date = NULL;
			char *kernel_ver = NULL;
			int mode = 0;
			int test_data_collected = 0;
			int is_malformed = 0;
			size_t system_info_cap = BUF_SIZE;
			size_t system_info_len = 0;
			char *system_info = malloc(system_info_cap);
			if (!system_info) {
				fprintf(stderr, "Failed to malloc system_info buffer\n");
				is_malformed = 1;
				break;
			}
			system_info[0] = '\0';

			while (fgets(line, sizeof(line), f)) {
				size_t line_len = strlen(line);

				if (test_data_collected) {
					if (system_info_len + line_len + 1 > system_info_cap) {
						size_t new_cap = system_info_cap * 2;
						if (new_cap < system_info_len + line_len + 1)
							new_cap = system_info_len + line_len + 1;

						char *tmp = realloc(system_info, new_cap);
						if (!tmp) {
							fprintf(stderr, "Failed to expand system_info buffer\n");
							is_malformed = 1;
							break;
						}

						system_info = tmp;
						system_info_cap = new_cap;
					}

					memcpy(system_info + system_info_len, line, line_len);
					system_info_len += line_len;
					system_info[system_info_len] = '\0';
				}
				else if (strspn(line, " \t\n\v\f\r") == line_len)
					continue;
				else if (strncmp(line, "Name: ", 6) == 0)
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
					date = strndup(line + 6, (line_len - 7));
				else if (strncmp(line, "System:    Kernel: ", 19) == 0) {
					const char *p = line + 19;
					const char *end = strchr(p, ' ');
					kernel_ver = strndup(p, end - p);
					memcpy(system_info + system_info_len, line, line_len);
					system_info_len += line_len;
					system_info[system_info_len] = '\0';
					test_data_collected = 1;
					continue;
				}
				else {
					char *test_name = NULL;
					char *test_result_str = NULL;
					double test_result = 0;

					char *col = strchr(line, ':');
					if (!col) {
						fprintf(stderr, "Failed to read test name (no colon): %s\n", line);
						is_malformed = 1;
						continue;
					}

					test_name = strndup(line, col - line);
					if (!test_name) {
						fprintf(stderr, "Failed to read test name from: %s\n", line);
						is_malformed = 1;
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

					test_result_str = strndup(line + strlen(test_name) + 2, line_len);
					if (!test_result_str) {
						fprintf(stderr, "Failed to read result from: %s\n", line);
						continue;
					}

					if (!parse_result(test_result_str, &test_result)) {
						fprintf(stderr, "Test: '%s' has invalid numeric: %s\n", test_name, test_result_str);
						is_malformed = 1;
					}

					free(test_result_str);

					t_test_entry *node = new_test_entry(test_name, test_result);
					if (!node) {
						fprintf(stderr, "Memory allocation failed for test entry '%s'\n", test_name);
						free(test_name);
						clean_test_entries(te);
						free(date);
						free(kernel_ver);
						free(system_info);
						if (fclose(f) != 0) fprintf(stderr, "Failed to close file\n");
						continue;
					}

					test_entry_add_back(&te, node);
				}
			}

			if (!date || !kernel_ver)
				is_malformed = 1;

			if (kernel_filter && kernel_ver) {
				if (!is_on_filterlist(kernel_ver, kernel_filter)) {
					free(kernel_ver);
					free(date);
					free(system_info);
					clean_test_entries(te);
					if (fclose(f) != 0) {
						fprintf(stderr, "Failed to close file\n");
						clean_benchmarks(bm);
						if (closedir(dir) != 0)
							fprintf(stderr, "Failed to close directory\n");
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
					clean_test_entries(te);
					if (fclose(f) != 0) {
						fprintf(stderr, "Failed to close file\n");
						clean_benchmarks(bm);
						if (closedir(dir) != 0)
							fprintf(stderr, "Failed to close directory\n");
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
					if (closedir(dir) != 0)
						fprintf(stderr, "Failed to close directory\n");
					return NULL;
				}
				continue;
			}

			t_benchmark *bench = new_benchmark(te, date, kernel_ver, system_info, mode);
			if (!bench) {
				fprintf(stderr, "Memory allocation failed for benchmark node\n");
				clean_test_entries(te);
				free(date);
				free(kernel_ver);
				free(system_info);
				if (fclose(f) != 0) fprintf(stderr, "Failed to close file\n");
				continue;
			}
			benchmark_add_back(&bm, bench);

			if (is_malformed) {
				fprintf(stderr, "Malformed log file: %s\n", entry->d_name);
				if (fclose(f) != 0)
					fprintf(stderr, "Failed to close file\n");
				if (closedir(dir) != 0)
					fprintf(stderr, "Failed to close directory\n");
				clean_benchmarks(bm);
				return NULL;
			}
			else if (fclose(f) != 0) {
				fprintf(stderr, "Failed to close file\n");
				clean_benchmarks(bm);
				if (closedir(dir) != 0)
					fprintf(stderr, "Failed to close directory\n");
				return NULL;
			}
		}
	}

	if (closedir(dir) != 0) {
		fprintf(stderr, "Failed to close directory\n");
		clean_benchmarks(bm);
		return NULL;
	};

	return bm;
}
