# @Author: Engeryu
# @Date:   2018-05-04 17:49:17
# @Last Modified by:   Engeryu
# @Last Modified time: 2025-03-30 23:42:49
# @Description: This Makefile builds the static library "libmy.a" from the source files
# provided in the project and also compiles an executable ("my_ls") that links to the library.
# The compilation is performed using gcc with the C11 standard and the following flags:
#   -Wall -Werror -Wextra -std=c11 -O2
#
# Targets:
#   all   - Builds the library.
#   clean - Removes object files.
#   fclean- Removes object files and the library.
#   re    - Rebuilds everything.
#
# Note: This Makefile automatically gathers all C source files in the lib directory.

CC      = gcc
NAME    = lib/libmy.a
SRCDIR  = lib
INCDIR  = include

# Automatically retrieve all C source files in the lib directory.
SRC     = $(wildcard $(SRCDIR)/*.c)
OBJ     = $(SRC:.c=.o)
DEPS    = $(OBJ:.o=.d)

# Define the primary executable's source file.
MAIN    = my_ls.o
MAINAUTODEPS = $(MAIN:.o=.d)

CPPFLAGS += -I$(INCDIR)
CFLAGS   += -Wall -Werror -Wextra -std=c11 -O2 -MMD -MP

# Allow make to search for sources and headers in these directories.
VPATH    = $(SRCDIR) $(INCDIR)

all: $(NAME) my_ls

$(NAME): $(OBJ)
		ar -crs $(NAME) $(OBJ)

# Pattern rule for objects in lib
$(SRCDIR)/%.o: $(SRCDIR)/%.c
		$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Compile my_ls.c to an object to generate its dependency file.
my_ls.o: my_ls.c
		$(CC) $(CFLAGS) $(CPPFLAGS) -c my_ls.c -o my_ls.o

# Link the final executable.
my_ls: $(MAIN) $(NAME)
		$(CC) $(CFLAGS) $(MAIN) -Llib -lmy -o my_ls

clean:
		rm -f $(OBJ) $(DEPS) $(MAIN) $(MAINAUTODEPS)

fclean: clean
		rm -f $(NAME) my_ls

re: fclean all

.PHONY: all clean fclean re