/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log_parser.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmakkone <jmakkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 10:36:12 by jmakkone          #+#    #+#             */
/*   Updated: 2025/04/19 00:00:35 by jmakkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOG_PARSER_H
#define LOG_PARSER_H

#define BUF_SIZE 1024

#include "benchmark.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

t_benchmark *read_logs(const char *path, const char *kernel_filter, const char *test_filter,
					const char *exclude_kernel_filter, const char *exclude_test_filter);

#endif
