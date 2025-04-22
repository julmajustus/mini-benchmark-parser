/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_benchmarks.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmakkone <jmakkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 10:34:48 by jmakkone          #+#    #+#             */
/*   Updated: 2025/04/22 03:05:46 by jmakkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PRINT_BENCHMARKS_H
#define PRINT_BENCHMARKS_H

#include "benchmark.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void	print_benchmark(t_benchmark *bench, int print_sys_info);
void	print_kernel_comparison(t_benchmark *combined);

#endif
