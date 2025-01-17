# Makefile for OpenMP Jacobi Solver

CC = gcc
OPT_FAST = -O3 -march=native -fopenmp
OPT_NONE = -O0 -fopenmp

TARGET_J = poisson_j        # Unoptimized Jacobi Solver
TARGET_J_OPT = poisson_j_opt  # Optimized Jacobi Solver

SOURCES = main.c print.c alloc3d.c jacobi.c

# Object files for each build (unoptimized vs optimized)
OBJECTS = main.o print.o alloc3d.o jacobi.o
OBJECTS_OPT = main_opt.o print_opt.o alloc3d_opt.o jacobi_opt.o

# Rule to build both programs
all: $(TARGET_J) $(TARGET_J_OPT)

# Optimized version (-O3)
$(TARGET_J_OPT): $(OBJECTS_OPT)
	$(CC) $(OPT_FAST) -o $@ $(OBJECTS_OPT) -lm

# Unoptimized version (-O0)
$(TARGET_J): $(OBJECTS)
	$(CC) $(OPT_NONE) -o $@ $(OBJECTS) -lm

# Compilation rules for standard (-O0)
%.o: %.c
	$(CC) $(OPT_NONE) -D_JACOBI -D_JACOBI_PARALLEL -D_JACOBI_PARALLEL_OPT -c $< -o $@

# Compilation rules for optimized (-O3)
%_opt.o: %.c
	$(CC) $(OPT_FAST) -D_JACOBI -D_JACOBI_PARALLEL -D_JACOBI_PARALLEL_OPT -c $< -o $@

clean:
	rm -f *.o $(TARGET_J) $(TARGET_J_OPT)

realclean: clean
	rm -f jacobi_results.data