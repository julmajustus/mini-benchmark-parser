/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmakkone <jmakkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 15:12:30 by jmakkone          #+#    #+#             */
/*   Updated: 2025/04/22 02:28:28 by jmakkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "benchmark.h"
#include "log_parser.h"
#include "print_benchmarks.h"
#include "generate_charts.h"
#include <getopt.h>
#include <stdio.h>

static void print_usage(const char *binary_name)
{
    printf("Usage: %s [OPTIONS]\n\n", binary_name);
    printf("Options:\n");
    printf("  -h, --help                  Show this help message and exit\n");
    printf("  -p, --log-path <directory>  Specify the directory containing log files (default: .)\n");
    printf("  -f, --include-test <str>    Include only tests matching the given pattern\n");
    printf("  -F, --include-kernel <str>  Include only kernel versions matching the given pattern\n");
    printf("  -e, --exclude-test <str>    Exclude tests matching the given pattern\n");
    printf("  -E, --exclude-kernel <str>  Exclude kernel versions matching the given pattern\n");
    printf("  -b, --bench                 Print all matching benchmark results\n");
    printf("  -c, --combine               Combine duplicate kernel data and calculate averages\n");
    printf("  -k, --compare-kernels       Print kernel performance comparisons\n");
    printf("  -g, --generate-chart        Generate kernel comparison charts\n");
    printf("  -G, --make-html             Generate an HTML page for the charts\n");
    printf("  -i, --info                  Display system information\n");
    printf("\nExamples:\n");
    printf("  %s --log-path /var/logs --include-test \"Test1|Test2\" \\\n", binary_name);
    printf("     --include-kernel \"6.13.4-gentoo-dist|6.14.0-gentoo-dist\" --bench\n");
    printf("  %s --combine --info --log-path .local/share/minibenchmark\n", binary_name);
    printf("  %s --log-path .local/share/minibenchmark -gG -e \"Total time (s)|Total score\"\n", binary_name);
}

int main(int ac, char **av)
{
	int opt;
	int get_benchmarks = 0, get_combined_benchmarks = 0, get_kernel_comparisons = 0, print_sys_info = 0, get_charts = 0, make_html = 0;
	char *path = ".";
	char *test_filter = NULL;
	char *kernel_filter = NULL;
	char *exclude_test_filter = NULL;
	char *exclude_kernel_filter = NULL;
	t_benchmark *benchmarks;

	struct option long_options[] = {
		{"help",                    no_argument,       0, 'h'},
		{"log-path",                required_argument, 0, 'p'},
		{"include-test",            required_argument, 0, 'f'},
		{"include-kernel",          required_argument, 0, 'F'},
		{"exclude-test",            required_argument, 0, 'e'},
		{"exclude-kernel",          required_argument, 0, 'E'},
		{"bench",                   no_argument,       0, 'b'},
		{"combine",                 no_argument,       0, 'c'},
		{"compare-kernels",         no_argument,       0, 'k'},
		{"generate-chart",          no_argument,       0, 'g'},
		{"make-html",               no_argument,       0, 'G'},
		{"info",                    no_argument,       0, 'i'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(ac, av, "hp:f:F:e:E:bckigG", long_options, NULL)) != -1) {
		switch (opt) {
			case 'h':
				print_usage(av[0]);
				return 0;
			case 'b':
				get_benchmarks = 1;
				break;
			case 'c':
				get_combined_benchmarks = 1;
				break;
			case 'k':
				get_kernel_comparisons = 1;
				break;
			case 'i':
				print_sys_info = 1;
				break;
			case 'p':
				path = optarg;
				break;
			case 'f':
				test_filter = optarg;
				break;
			case 'F':
				kernel_filter = optarg;
				break;
			case 'e':
				exclude_test_filter = optarg;
				break;
			case 'E':
				exclude_kernel_filter = optarg;
				break;
			case 'g':
				get_charts = 1;
				break;
			case 'G':
				make_html = 1;
				break;
			default:
				print_usage(av[0]);
				return 1;
		}
	}

	benchmarks = read_logs(path, kernel_filter, test_filter, exclude_kernel_filter, exclude_test_filter);
	if (!benchmarks) {
		fprintf(stderr, "Failed to read logs\n");
		return 1;
	}

	if (get_benchmarks)
		print_benchmark(benchmarks, print_sys_info);

	if (get_combined_benchmarks || get_kernel_comparisons) {
		t_benchmark *combined = combine_benchmarks(benchmarks);
		if (!combined) {
			fprintf(stderr, "Benchmark combine failed\n");
			clean_benchmarks(benchmarks);
			return 1;
		}
		combined = sort_benchmarks(combined);
		if (get_combined_benchmarks)
			print_benchmark(combined, print_sys_info);
		if (get_kernel_comparisons) {
			print_kernel_comparison(combined);
		}
		clean_benchmarks(combined);
	}

	if (get_charts) {
		const char *home = getenv("HOME");

		if (home) {
			char path[PATH_MAX];
			snprintf(path, sizeof(path), "%s/.local/share/mbparser/", home);

			if (access(path, F_OK | R_OK | W_OK) != 0) {
				if (mkdir(path, 0755) == -1) {
					fprintf(stderr, "Failed to create the directory: %s\n", path);
					clean_benchmarks(benchmarks);
					return 1;
				}
			}
			if (chdir(path) != -1)
				generate_comparison_charts(benchmarks, make_html);
			else
				fprintf(stderr, "Failed to change directory!\n");
		}
		else
			fprintf(stderr, "Getenv failed to get $HOME variable\n");
	}

	clean_benchmarks(benchmarks);

	return 0;
}
