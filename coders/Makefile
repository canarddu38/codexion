CFLAGS		= -Wall -Wextra -Werror -pthread
NAME		= codexion
CFILES		= main.c \
			  coder.c \
			  coder_utils.c \
			  monitor.c \
			  data_structures.c \
			  parser.c

OFILES		= $(CFILES:.c=.o)
DFILES		= $(CFILES:.c=.d)

all: $(NAME)

$(NAME): $(OFILES)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OFILES) $(DFILES)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
-include $(DFILES)