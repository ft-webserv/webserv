# ---------------------------------------------------------------------------- #
#	Define the compiler and flags											   #
# ---------------------------------------------------------------------------- #

CC					:= c++
# CFLAGS				:= -std=c++98 -Werror -Wextra -Wall
CPPFLAGS			= -I includes
MJ					= -MJ $(patsubst $(BUILD_DIR)/$(OBJ_DIR)/%.o, $(BUILD_DIR)/$(JSON_DIR)/%.part.json, $@)

# ---------------------------------------------------------------------------- #
#	Define the directories													   #
# ---------------------------------------------------------------------------- #

SRC_DIR				:= sources

CONFIG_DIR			:= Config
SERVER_DIR			:= Server
KQUEUE_DIR			:= Kqueue
SERVERMANAGER_DIR	:= ServerManager
REQUEST_DIR			:= Request
RESPONSE_DIR		:= Response
SERVERINFO_DIR		:= ServerInfo
EXCEPTION_DIR		:= Exception
CLIENT_DIR			:= Client
CGI_DIR				:= CGI
ERROR_DIR			:= Error
UTILS_DIR			:= Utils

BUILD_DIR			:= build
OBJ_DIR				:= obj
JSON_DIR			:= json

# ---------------------------------------------------------------------------- #
#	Define the source files													   #
# ---------------------------------------------------------------------------- #

SRCS				:= $(addprefix $(SRC_DIR)/, main.cpp)
SRCS				+= $(addprefix $(SRC_DIR)/$(CONFIG_DIR)/, Config.cpp)
SRCS				+= $(addprefix $(SRC_DIR)/$(SERVER_DIR)/, Server.cpp)
SRCS				+= $(addprefix $(SRC_DIR)/$(KQUEUE_DIR)/, Kqueue.cpp)
SRCS				+= $(addprefix $(SRC_DIR)/$(SERVERMANAGER_DIR)/, ServerManager.cpp)
SRCS				+= $(addprefix $(SRC_DIR)/$(REQUEST_DIR)/, Request.cpp)
SRCS				+= $(addprefix $(SRC_DIR)/$(RESPONSE_DIR)/, Response.cpp)
SRCS				+= $(addprefix $(SRC_DIR)/$(SERVERINFO_DIR)/, ServerInfo.cpp LocationInfo.cpp)
SRCS				+= $(addprefix $(SRC_DIR)/$(EXCEPTION_DIR)/, Exception.cpp)
SRCS				+= $(addprefix $(SRC_DIR)/$(CLIENT_DIR)/, Client.cpp)
SRCS				+= $(addprefix $(SRC_DIR)/$(CGI_DIR)/, Cgi.cpp)
SRCS				+= $(addprefix $(SRC_DIR)/$(ERROR_DIR)/, Error.cpp)
SRCS				+= $(addprefix $(SRC_DIR)/$(UTILS_DIR)/, Utils.cpp)

OBJS				:= $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/$(OBJ_DIR)/%.o, $(SRCS))

# ---------------------------------------------------------------------------- #
#	Define the variables for progress bar									   #
# ---------------------------------------------------------------------------- #

TOTAL_FILES			:= $(shell echo $(SRCS) | wc -w)
COMPILED_FILES		:= 0
STEP				:= 100

# ---------------------------------------------------------------------------- #
#	Define the target														   #
# ---------------------------------------------------------------------------- #

NAME				:= webserv
JSON				:= compile_commands.json

# ---------------------------------------------------------------------------- #
#	Define the rules														   #
# ---------------------------------------------------------------------------- #

all:
		@$(MAKE) $(NAME)

$(NAME): $(OBJS)
		@$(CC) $(CFLAGS) $(OBJS) -o $@
		@printf "\n$(MAGENTA)[$(NAME)] Linking Success\n$(DEF_COLOR)"


$(BUILD_DIR)/$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | dir_guard
		@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@ $(MJ)
		@$(eval COMPILED_FILES = $(shell expr $(COMPILED_FILES) + 1))
		@$(eval PROGRESS = $(shell expr $(COMPILED_FILES) "*" $(STEP) / $(TOTAL_FILES)))
		@printf "$(SAVECUR) $(DELINE) $(YELLOW)[$(NAME)] [%02d/%02d] ( %3d %%) Compiling $<\r$(DEF_COLOR) $(RESTCUR)" $(COMPILED_FILES) $(TOTAL_FILES) $(PROGRESS)

clean:
		@$(RM) -r $(BUILD_DIR)
		@printf "$(BLUE)[$(NAME)] obj. files$(DEF_COLOR)$(GREEN)	=> Cleaned!\n$(DEF_COLOR)"

fclean:
		@$(RM) -r $(BUILD_DIR) $(NAME)
		@printf "$(BLUE)[$(NAME)] obj. files$(DEF_COLOR)$(GREEN)	=> Cleaned!\n$(DEF_COLOR)"
		@printf "$(CYAN)[$(NAME)] exec. files$(DEF_COLOR)$(GREEN)	=> Cleaned!\n$(DEF_COLOR)"

re:
		@$(MAKE) fclean
		@$(MAKE) all
		@printf "$(GREEN)[$(NAME)] Cleaned and rebuilt everything!\n$(DEF_COLOR)"

json:
		@$(MAKE) --always-make --keep-going all
		@(echo '[' && find . -name "*.part.json" | xargs cat && echo ']') > $(JSON)

dir_guard:
		@mkdir -p $(addprefix $(BUILD_DIR)/$(OBJ_DIR)/, $(CONFIG_DIR) $(SERVER_DIR) $(KQUEUE_DIR) $(SERVERMANAGER_DIR) $(REQUEST_DIR) $(RESPONSE_DIR) $(SERVERINFO_DIR) $(EXCEPTION_DIR) $(CLIENT_DIR) $(CGI_DIR) $(ERROR_DIR) $(UTILS_DIR))\
			$(addprefix $(BUILD_DIR)/$(JSON_DIR)/, $(CONFIG_DIR) $(SERVER_DIR) $(KQUEUE_DIR) $(SERVERMANAGER_DIR) $(REQUEST_DIR) $(RESPONSE_DIR) $(SERVERINFO_DIR) $(EXCEPTION_DIR) $(CLIENT_DIR) $(CGI_DIR) $(ERROR_DIR) $(UTILS_DIR))

.PHONY: all clean fclean re dir_guard

# ---------------------------------------------------------------------------- #
#	Define the colors														   #
# ---------------------------------------------------------------------------- #

DELINE = \033[K
CURSUP = \033[A
SAVECUR = \033[s
RESTCUR = \033[u

DEF_COLOR	= \033[1;39m
GRAY		= \033[1;90m
RED			= \033[1;91m
GREEN		= \033[1;92m
YELLOW		= \033[1;93m
BLUE		= \033[1;94m
MAGENTA		= \033[1;95m
CYAN		= \033[1;96m
WHITE		= \033[1;97m
