/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_charts.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmakkone <jmakkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 17:43:45 by jmakkone          #+#    #+#             */
/*   Updated: 2025/04/22 02:24:49 by jmakkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "generate_charts.h"

static t_benchmark *get_benchmarks_by_mode(const t_benchmark *benchmarks, int mode)
{
	const t_benchmark *bm = benchmarks;
	t_benchmark *group = NULL;
	t_benchmark *tail = NULL;
	
	while (bm) {
		if (bm->mode == mode) {
			t_benchmark *copy = malloc(sizeof(t_benchmark));
	
			if (!copy) {
				fprintf(stderr, "Memory allocation failed for benchmark copy.\n");
				continue;
			}

			memcpy(copy, bm, sizeof(t_benchmark));
			copy->next = NULL;

			if (!group) {
				group = copy;
				tail = copy;
			}
			else {
				tail->next = copy;
				tail = copy;
			}
		}
		bm = bm->next;
	}

	return group;
}

static char *generate_filename(const char *prefix, int mode)
{
	size_t size = 0;
	time_t rawtime;
	struct tm *timeinfo;
	char timestamp[64];

	const char *mode_str = (mode == 1 ? "mini" : mode == 2 ? "nano" : "");

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", timeinfo);

	if (mode)
		size = strlen(prefix) + strlen(mode_str) + strlen(timestamp) + 11;
	else
		size = strlen(prefix) + strlen(timestamp) + 11;

	char *filename = malloc(size);
	
	if (!filename) {
		fprintf(stderr, "Memory allocation failed for filename.\n");
		return NULL;
	}
	
	if (mode)
		snprintf(filename, size, "%s-%s_%s.png", prefix, mode_str, timestamp);
	else
		snprintf(filename, size, "%s_%s.png", prefix, timestamp);

	return filename;
}


static void plot_kernel_version_comparison(const t_benchmark *benchmarks, char *filename)
{
	if (!benchmarks || !filename)
		return;

	const t_benchmark *bm = benchmarks;
	PyObject *py_average_times = PyList_New(0);
	PyObject *py_kernel_versions = PyList_New(0);
	if (!py_average_times) {
		fprintf(stderr, "Failed to allocate Python objects.\n");
		return;
	}
	if (!py_kernel_versions) {
		fprintf(stderr, "Failed to allocate Python objects.\n");
		Py_DECREF(py_average_times);
		return;
	}

	while (bm) {
		PyObject *py_tests = PyDict_New();
		if (!py_tests) {
			fprintf(stderr, "Failed to allocate Python dict.\n");
			continue;
		}

		t_test_entry *te = bm->data;
		
		while (te) {
			PyObject *py_val = PyFloat_FromDouble(te->result);
			if (!py_val)
				continue;
			PyDict_SetItemString(py_tests, te->name, py_val);
			Py_DECREF(py_val);
			te = te->next;
		}

		PyList_Append(py_average_times, py_tests);
		Py_DECREF(py_tests);

		PyObject *py_kv = PyUnicode_FromString(bm->kernel_ver);	
		if (!py_kv)
			continue;
		
		PyList_Append(py_kernel_versions, py_kv);
		Py_DECREF(py_kv);
		bm = bm->next;
	}

	PyList_Reverse(py_kernel_versions);
	PyList_Reverse(py_average_times);

	const char *plot_code =
		"import matplotlib.pyplot as plt\n"
		"import numpy as np\n"
		"import matplotlib.colors as mcolors\n"
		"\n"
		"def plot_kernel_comparison(average_times, kernel_versions):\n"
		"    test_names = list(average_times[0].keys())\n"
		"    test_names.reverse()\n"
		"    num_tests = len(test_names)\n"
		"    num_kernel_versions = len(kernel_versions)\n"
		"    base_height_per_test = 0.8\n"
		"    additional_height_per_kernel = 1.8\n"
		"    fig_height = base_height_per_test * num_tests + additional_height_per_kernel * num_kernel_versions\n"
		"    fig_width = 12\n"
		"    fig, ax = plt.subplots(figsize=(fig_width, fig_height))\n"
		"    bar_height = 0.8 / num_kernel_versions\n"
		"    colors = list(mcolors.TABLEAU_COLORS.keys())\n"
		"    font_size = max(6, 16 - num_kernel_versions * 0.5)\n"
		"    for i, avg_times in enumerate(average_times):\n"
		"        values = list(avg_times.values())[::-1]\n"
		"        color = colors[i % len(colors)]\n"
		"        ax.barh(np.arange(num_tests) + i * bar_height, values, height=bar_height,\n"
		"                label=kernel_versions[i], color=color)\n"
		"        for j, value in enumerate(values):\n"
		"            ax.text(value, j + i * bar_height, f'{value:.2f}', fontsize=font_size,\n"
		"                    ha='left', va='center', color='black')\n"
		"    ax.set_yticks(np.arange(num_tests) + bar_height * (num_kernel_versions - 1) / 2)\n"
		"    ax.set_yticklabels(test_names)\n"
		"    ax.set_xlabel('Average Time (s). Less is better')\n"
		"    ax.set_ylabel('Mini-Benchmarker')\n"
		"    ax.set_title(f'Test Performance Comparison Between Different Kernel Versions')\n"
		"    handles, labels = ax.get_legend_handles_labels()\n"
		"    ax.legend(handles[::-1], labels[::-1], loc='lower right')\n"
		"    ax.grid(axis='x')\n"
		"    plt.tight_layout()\n"
		"    plt.savefig(filename)\n"
		"    plt.close()\n"
		"\n"
		"plot_kernel_comparison(average_times, kernel_versions)\n";

	PyObject *py_main_dict = PyModule_GetDict(PyImport_AddModule("__main__"));
	PyDict_SetItemString(py_main_dict, "average_times", py_average_times);
	PyDict_SetItemString(py_main_dict, "kernel_versions", py_kernel_versions);

	PyObject *py_fname = PyUnicode_FromString(filename);
	if (py_fname) {
		PyDict_SetItemString(py_main_dict, "filename", py_fname);
		Py_DECREF(py_fname);
	}

	if (PyRun_SimpleString(plot_code) != 0) {
		fprintf(stderr, "Error executing embedded Python plot code.\n");
		PyErr_Print();
		Py_DECREF(py_average_times);
		Py_DECREF(py_kernel_versions);
		return;
	}

	printf("Comparison chart generated successfully as '~/.local/share/mbparser/%s'!\n", filename);

	Py_DECREF(py_average_times);
	Py_DECREF(py_kernel_versions);

}

static void generate_html_page(char **filenames)
{
	if (!filenames[0] && !filenames[1] && !filenames[2]) {
		fprintf(stderr, "No charts to write, skipping HTML\n");
		return;
	}

	FILE *fp = fopen("test_performance.html", "w");
	if (!fp) {
		fprintf(stderr, "Failed to open test_performance.html for writing\n");
		return;
	}

	fprintf(fp,
		 "<!DOCTYPE html>\n"
		 "<html lang=\"en\">\n"
		 "<head>\n"
		 "    <meta charset=\"UTF-8\">\n"
		 "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
		 "    <title>Test Performance</title>\n"
		 "</head>\n"
		 "<body>\n"
		 "    <h1>Test Performance</h1>\n");

	for (int i = 0; i < 3; i++) {
		if (filenames[i]) {
			char *mode = "";
			if (i > 0)
				mode = i == 1 ? " Test Mode: mini" : " Test Mode: nano";
			fprintf(fp,
		   "    <h2>Performance Comparison Between Different Kernel Versions%s</h2>\n"
		   "    <img src=\"%s\" alt=\"Performance Comparison Between Different Kernel Versions - All Kernels\" style=\"max-width: 100%%; height: auto;\">\n",
		   mode, filenames[i]);
		}
	}

	fprintf(fp,
		 "</body>\n"
		 "</html>\n");

	fclose(fp);
	printf("HTML page generated successfully as '~/.local/share/mbparser/test_performance.html'!\n");
}

void generate_comparison_charts(t_benchmark *benchmarks, int make_html)
{

	char **filename = malloc(sizeof(char *) * 3);
	if (!filename) {
		fprintf(stderr, "Memory allocation error.\n");
		return;
	}

	for (int i = 0; i < 3; i++) {
		filename[i] = NULL;
	}
	
	Py_Initialize();
	for (int i = 0; i < 3; i++) {
		t_benchmark *bm = get_benchmarks_by_mode(benchmarks, i);
		if (!bm)
			continue;

		filename[i] = generate_filename("kernel_version_comparison", i);
		if (!filename[i]) {
			fprintf(stderr, "Failed to generate filename for mode: %d\n", i);
			while (bm) {
				t_benchmark *next = bm->next;
				free(bm);
				bm = next;
			}
			continue;
		}
		
		t_benchmark *combined = combine_benchmarks(bm);
		if (!combined) {
			fprintf(stderr, "Benchmark combine failed\n");
			free(filename);
			while (bm) {
				t_benchmark *next = bm->next;
				free(bm);
				bm = next;
			}
			Py_Finalize();
			return;
		}

		combined = sort_benchmarks(combined);
		plot_kernel_version_comparison(combined, filename[i]);
		clean_benchmarks(combined);

		while (bm) {
			t_benchmark *next = bm->next;
			free(bm);
			bm = next;
		}
	}

	Py_Finalize();

	if (make_html)
		generate_html_page(filename);
	
	for (int i = 0; i < 3; i++)
		free(filename[i]);
	free(filename);
}
