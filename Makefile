################################# DIRECTORIES ##################################
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = includes
LIB_DIR = libs

################################## LIBRAIRIES ##################################

LIBS =

###############################     INCLUDE      ###############################

IFLAGS = -Iincludes -Iincludes/core -Iincludes/server -Iincludes/parser -Iincludes/utils

############################### COMPILER & FLAGS ###############################

CXX = c++
CXX_VERSION = -std=c++98
CXX_FLAGS = -Wall -Werror -Wextra
GFLAG = -g3

############################# INPUT & OBJECT FILES #############################

EXTENSION_TYPE = cpp

# TODO: Update SRC_FILES
SRC_FILES =	$(shell find $(SRC_DIR) -type f -name "*.cpp")
# SRC_FILES = $(SRC_DIR)/main.cpp \


# Object files - map source files to object files
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.$(EXTENSION_TYPE)=$(BUILD_DIR)/%.o)

DEP_FILES = $(OBJ_FILES:.o=.d)


#############################       VALGRIND       #############################

VALG_FLAG	= valgrind --leak-check=full --show-leak-kinds=all \
				--track-origins=yes --track-fds=yes

################################## EXECUTABLE ##################################

EXEC = webserv

#################################### COLORS ####################################

DEFAULT_COLOR = \033[0;39m
RED_COLOR = \033[0;91m
GREEN_COLOR = \033[0;92m
YELLOW_COLOR = \033[0;93m
BLUE_COLOR = \033[0;94m

#################################### RULES #####################################

all: $(EXEC)

-include $(DEP_FILES)

$(LIBS):
	@$(MAKE) -C $(LIB_DIR) -s

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.$(EXTENSION_TYPE)
	@echo "$(YELLOW_COLOR)Compiling: $(DEFAULT_COLOR) $<"
	@mkdir -p $(@D)
	@$(CXX) $(CXX_VERSION) $(CXX_FLAGS) $(IFLAGS) -MMD -MP -c $< -o $@

$(EXEC): $(OBJ_FILES)
	@$(CXX) $(CXX_VERSION) $(CXX_FLAGS) $(LIBS) $(OBJ_FILES) -o $@
	@echo "$(GREEN_COLOR)Executable: $(DEFAULT_COLOR)$(EXEC) created!✅"

valgrind: $(EXEC)
	$(VALG_FLAG) ./$(EXEC)

clean:
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN_COLOR)Cleaned object files.$(DEFAULT_COLOR) 🧹"

fclean: clean
	@rm -f $(EXEC)
	@echo "$(GREEN_COLOR)Removed executable.$(DEFAULT_COLOR) 🧹"

re: fclean all

debugger: CXX_FLAGS += $(GFLAG)
debugger: re

.PHONY: all clean fclean re debugger valgrind
