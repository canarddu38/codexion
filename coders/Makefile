CFLAGS		= -Wall -Wextra -Werror -std=c89 -pthread
NAME		= codexion
CFILES		= main.c \
			  coder.c \
			  coder_utils.c \
			  monitor.c \
			  data_structures.c \
			  parser.c

OBJDIR		= obj
OFILES		= $(CFILES:%.c=$(OBJDIR)/%.o)
DFILES		= $(CFILES:%.c=$(OBJDIR)/%.d)

all: $(NAME)

$(NAME): $(OFILES)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(DFILES)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
-include $(DFILES)