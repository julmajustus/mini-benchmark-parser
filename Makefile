# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jmakkone <jmakkone@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/08 15:31:00 by jmakkone          #+#    #+#              #
#    Updated: 2025/04/26 00:14:00 by jmakkone         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = mbparser
SRC_DIR     = src
INC_DIR     = include
OBJ_DIR     = obj

SRC         = $(SRC_DIR)/main.c \
              $(SRC_DIR)/benchmark.c \
              $(SRC_DIR)/test_entry.c \
              $(SRC_DIR)/log_parser.c \
              $(SRC_DIR)/print_benchmarks.c \
              $(SRC_DIR)/generate_charts.c

OBJ         = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

CC          = gcc

ifndef BUILD
	BUILD = release
endif

ifeq ($(BUILD), debug)
    CFLAGS := -Wall -Wextra -Werror -Og -ggdb3
else ifeq ($(BUILD), release)
    CFLAGS := -Wall -Wextra -Werror -O3 -march=native
else
    CFLAGS := -Wall -Wextra -Werror -O2
endif

PYTHON ?= python3

PYTHON_CFLAGS := $(shell $(PYTHON)-config --cflags --embed)
PYTHON_LDFLAGS := $(shell $(PYTHON)-config --ldflags --embed)

CFLAGS  += -I$(INC_DIR) $(PYTHON_CFLAGS)
LDLIBS  += $(PYTHON_LDFLAGS)

RM          = rm -f

INSTALL_DIR = /usr/local/bin

.PHONY: all clean fclean re debug release install uninstall

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

clean:
	@echo "Cleaning objects..."
	$(RM) -r $(OBJ_DIR)

fclean: clean
	@echo "Removing binary..."
	$(RM) $(NAME)

re: fclean all

debug:
	$(MAKE) BUILD=debug all

release:
	$(MAKE) BUILD=release all

install: $(NAME)
	@echo "Installing $(NAME) to $(INSTALL_DIR)..."
	@cp $(NAME) $(INSTALL_DIR)/$(NAME)
	@echo "Installation complete."

uninstall:
	@echo "Uninstalling $(NAME) from $(INSTALL_DIR)..."
	@rm -f $(INSTALL_DIR)/$(NAME)
	@echo "Uninstallation complete."
