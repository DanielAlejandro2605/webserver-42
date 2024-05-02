# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dnieto-c <dnieto-c@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/31 22:47:31 by mflores-          #+#    #+#              #
#    Updated: 2024/05/02 17:14:18 by dnieto-c         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#------------------------------------------------------------------------------#
#	GENERAL																	   #
#------------------------------------------------------------------------------#

NAME		= webserv
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++11
LDFLAGS 	= -lcriterion
RM			= rm -f

#------------------------------------------------------------------------------#
#	HEADER FILES				           				    				   #
#------------------------------------------------------------------------------#

HEADERS_PATH 	= includes/
HEADER_FILES	= Cluster Server HandlerRequest Request HandlerServer \
					HandlerResponse Dispatcher webserv 

CONFIG_HEADERS	= $(addprefix Config/, CGI Config ContextBase HandlerContext \
									Location Parser ServerContext)

UTILS_HEADERS	= $(addprefix utils/, colors httpCodes utils)

HEADERS = $(addsuffix .hpp, $(addprefix $(HEADERS_PATH), $(HEADER_FILES))) \
          $(addsuffix .hpp, $(addprefix $(HEADERS_PATH), $(CONFIG_HEADERS))) \
          $(addsuffix .hpp, $(addprefix $(HEADERS_PATH), $(UTILS_HEADERS)))

HEADERS_INC		= $(addprefix -I, $(HEADERS_PATH))

#------------------------------------------------------------------------------#
#	WEBSERV FILES														   	   #
#------------------------------------------------------------------------------#

# List of all .cpp source files
ROOT_FILES = main Cluster HandlerRequest Request Server HandlerServer HandlerResponse Dispatcher

UTILS_FILES = utils
UTILS_FOLDER = utils/

CONFIG_FILES = CGI Config ContextBase HandlerContext Location Parser ServerContext
CONFIG_FOLDER = Config/

#------------------------------------------------------------------------------#
#	BENCHMAKING FILES													   	   #
#------------------------------------------------------------------------------#

BENCHMAKING_FILES = t_parser t_function
BENCHMAKING_FOLDER = benchmarking/
BENCHMAKING_EXEC = test

CRITERION_INCLUDE = -I/usr/local/include/criterion
CRITERION_LINK = -lcriterion

SRCS_PATH = srcs/
SRCS_FILES 	= $(addsuffix .cpp, $(ROOT_FILES) \
							$(addprefix $(UTILS_FOLDER), $(UTILS_FILES)) \
							$(addprefix $(CONFIG_FOLDER), $(CONFIG_FILES)) \
							$(addprefix $(BENCHMAKING_FOLDER), $(BENCHMAKING_FILES)))


# All .o files go to objs/ directory
OBJS_PATH 	= objs/
OBJS_NAMES	= $(SRCS_FILES:.cpp=.o)
OBJS_FOLDER = $(addprefix $(OBJS_PATH), $(UTILS_FOLDER) $(CONFIG_FOLDER), $(BENCHMAKING_FOLDER)) 
OBJS		= $(addprefix $(OBJS_PATH), $(OBJS_NAMES))

# c++ will create these .d files containing dependencies
DEPS		= $(addprefix $(OBJS_PATH), $(SRCS_FILES:.cpp=.d))

#------------------------------------------------------------------------------#
#	BASCIC RULES															   #
#------------------------------------------------------------------------------#

all: header $(NAME) $(BENCHMAKING_EXEC)
	@echo "\n$(BOLD)$(GREEN)\n[ ✔ ]\t$(NAME)\n$(WHITE)"
	@echo "$(BOLD)$(YELLOW)▶ TO LAUNCH:\t$(WHITE)./$(NAME) <config file>\n$(RESET)"

$(NAME): $(HEADERS) $(OBJS)
	@$(CXX) $(CXXFLAGS) $(HEADERS_INC) $(CRITERION_INCLUDE) $(CRITERION_LINK) $(OBJS) -o $(NAME)

$(BENCHMAKING_EXEC): $(filter-out $(OBJS_PATH)main.o, $(OBJS))
	@echo "\n$(BOLD)$(GREEN)\n[ ✔ ]\t$(BENCHMAKING_EXEC)\n$(WHITE)"
	@$(CXX) $(CXXFLAGS) $(HEADERS_INC) $^ -o $@ $(CRITERION_INCLUDE) $(CRITERION_LINK)

$(OBJS_PATH)%.o: $(SRCS_PATH)%.cpp
	@mkdir -p $(OBJS_FOLDER)
	@$(CXX) $(CXXFLAGS) $(HEADERS_INC) -MMD -MP -MT $@ -o $@ -c $<
	@printf "$(YELLOW). . . compiling $(NAME) objects . . . $(ITALIC)$(GREY)%-33.33s\r$(RESET)" $@

clean:
	@if [ -d $(OBJS_PATH) ]; then \
		echo "$(YELLOW)\n. . . cleaning objects . . .$(RESET)"; \
		$(RM) -rd $(OBJS_PATH); \
	fi
	@echo "$(BOLD)$(GREEN)\n[ ✔ ]\tOBJECTS CLEANED$(RESET)"

fclean:	clean
	@if [ -e $(NAME) ]; then \
		echo "$(YELLOW)\n. . . cleaning rest . . .$(RESET)"; \
		$(RM) $(NAME); \
		$(RM) $(BENCHMAKING_EXEC); \
	fi
	@echo "$(BOLD)$(GREEN)\n[ ✔ ]\tALL CLEANED\n$(RESET)"

re:	fclean all

# Include all .d files
-include $(DEPS)

.PHONY:	all clean fclean re header

#------------------------------------------------------------------------------#
#	CUSTOM RULES															   #
#------------------------------------------------------------------------------#
GITHUB_PROF = https://github.com/mariav7
GITHUB_COLL = https://github.com/DanielAlejandro2605

define HEADER_PROJECT
	    __    __     _
	   / / /\ \ \___| |__  ___  ___ _ ____   __
	   \ \/  \/ / _ \ '_ \/ __|/ _ \ '__\ \ / /
	    \  /\  /  __/ |_) \__ \  __/ |   \ V /
	     \/  \/ \___|_.__/|___/\___|_|    \_/ 
                                        
endef
export HEADER_PROJECT

header:
	clear
	@printf "$(CYAN) $(DBLUE) $$HEADER_PROJECT $(RESET)\n"
	@printf "$(MAGENTA)%15s Coded by:$(WHITE) \e]8;;$(GITHUB_COLL)\e\\dnieto-c\e]8;;\e\\ \
	$(MAGENTA)and$(WHITE) \e]8;;$(GITHUB_PROF)\e\\\mflores-\e]8;;\e\\ $(RESET)\n\n"

# COLORS
RESET = \033[0m
WHITE = \033[37m
GREY = \033[90m
RED = \033[91m
DRED = \033[31m
GREEN = \033[92m
DGREEN = \033[32m
YELLOW = \033[93m
DYELLOW = \033[33m
BLUE = \033[94m
DBLUE = \033[34m
MAGENTA = \033[95m
DMAGENTA = \033[35m
CYAN = \033[96m
DCYAN = \033[36m

# FORMAT
BOLD = \033[1m
ITALIC = \033[3m
BLINK = \033[5m
UNDERLINE = \033[4m
STRIKETHROUGH = \033[9m

# NOTES:
# ex. 	@$(CXX) $(CXXFLAGS) $(HEADERS_INC) -MMD -MP -MT $@ -o $@ -c $<
# -MMD: Generates dependency information in the form of Makefile rules. This is 
# used to automatically track changes in header files and rebuild the corresponding object files
# -MP: Generates phony targets for header files, preventing errors if header files are deleted or renamed.
# -MT $@: Specifies the target for the rule, which is the object file being generated.
# This is important for proper handling of dependencies.
# -o $@: Specifies the output file, which is the object file being generated.
# In this case, it's the same as the target, indicating that the compiled object file will have the same name.
# -c $<: Indicates that the compiler should only perform compilation and not linking.
# $< represents the first prerequisite of the rule, which is the source file.