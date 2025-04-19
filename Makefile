# Compiler and flags
CC = gcc
CFLAGS = -I./include -Wall -g   # Include the 'include' directory for header files
LDFLAGS = -o bin/university_system # Set the output file path

# Source and object files
SRC = src/main.c src/auth_functions.c src/auth_system.c src/user_data.c src/attendance.c src/student_report.c src/utils.c
OBJ = $(SRC:.c=.o)   # Correctly substitute .c to .o

# Default target to build the project
all: bin/university_system

# Rule to compile the object files
bin/%.o: src/%.c
		$(CC) $(CFLAGS) -c $< -o $@ #Tab here

# Rule to link object files to create the executable
bin/university_system: $(OBJ)
		$(CC) $(OBJ) $(LDFLAGS) #Tab here

# Clean the build files
clean:
		rm -f $(OBJ) bin/university_system #Tab here
