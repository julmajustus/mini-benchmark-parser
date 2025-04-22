/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   generate_charts.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmakkone <jmakkone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 17:45:44 by jmakkone          #+#    #+#             */
/*   Updated: 2025/04/22 03:05:21 by jmakkone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GENERATE_CHARTS_H
#define GENERATE_CHARTS_H

#include <Python.h>
#include "benchmark.h"

void	generate_comparison_charts(t_benchmark *benchmarks, int make_html);

#endif
