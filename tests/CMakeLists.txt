cmake_minimum_required(VERSION 3.15)

project(minishell)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_library(ft)
target_sources(
	ft
	PUBLIC
		../Libft/libft.h
	PRIVATE
		../Libft/ft_memset.c
		../Libft/ft_bzero.c
		../Libft/ft_memcpy.c
		../Libft/ft_memccpy.c
		../Libft/ft_memmove.c
		../Libft/ft_memchr.c
		../Libft/ft_memcmp.c
		../Libft/ft_strlen.c
		../Libft/ft_strlcpy.c
		../Libft/ft_strlcat.c
		../Libft/ft_strchr.c
		../Libft/ft_strrchr.c
		../Libft/ft_strnstr.c
		../Libft/ft_strncmp.c
		../Libft/ft_atoi.c
		../Libft/ft_isalpha.c
		../Libft/ft_isdigit.c
		../Libft/ft_isalnum.c
		../Libft/ft_isascii.c
		../Libft/ft_isprint.c
		../Libft/ft_toupper.c
		../Libft/ft_tolower.c
		../Libft/ft_calloc.c
		../Libft/ft_strdup.c
		../Libft/ft_substr.c
		../Libft/ft_strjoin.c
		../Libft/ft_strtrim.c
		../Libft/ft_split.c
		../Libft/ft_itoa.c
		../Libft/ft_strmapi.c
		../Libft/ft_putchar_fd.c
		../Libft/ft_putstr_fd.c
		../Libft/ft_putendl_fd.c
		../Libft/ft_putnbr_fd.c
		../Libft/ft_arrcmp.c
		../Libft/ft_free_arr.c
)

# Link runTests with what we want to test and the GTest and pthread library
add_executable(runTests)
target_sources(
	runTests
	PUBLIC
		../includes/minishell.h
		../Libft/libft.h
	PRIVATE
		tests_count_tks.cpp
		tests_tokenizer.cpp
		tests_lexical_analyzer.cpp
		tests_sintax_analysis.cpp
		tests_clean_tokens.cpp
		../src/tokens/count_tks.c
		../src/tokens/tokenizer.c
		../src/tokens/lexical_analyzer.c
		../src/tokens/sintax_analysis.c
		../src/tokens/clean_tokens.c
		../src/init.c
)
target_link_libraries(
	runTests
	PRIVATE
		ft
	${GTEST_LIBRARIES}
	pthread
)
