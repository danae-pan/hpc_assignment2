# Makefile for OpenMP Jacobi Solver

CC = gcc
OPT_FAST = -O3 -march=native -fopenmp
OPT_NONE = -O0 -fopenmp

TARGET_J = poisson_j        # Unoptimized Jacobi Solver
TARGET_J_OPT = poisson_j_opt  # Optimized Jacobi Solver
TARGET_J_THREAD = poisson_j_thread  # Optimized Jacobi Solver

SOURCES = main.c print.c alloc3d.c jacobi.c

# Object files for each build (unoptimized vs optimized)
OBJECTS = main.o print.o alloc3d.o jacobi.o
OBJECTS_OPT = main_opt.o print_opt.o alloc3d_opt.o jacobi_opt.o
OBJECTS_THREAD = main_thread.o print_thread.o alloc3d_thread.o jacobi_thread.o

# Rule to build the three programs
all: $(TARGET_J) $(TARGET_J_OPT) $(TARGET_J_THREAD)

# Optimized version (-O3)
$(TARGET_J_OPT): $(OBJECTS_OPT)
	$(CC) $(OPT_FAST) -o $@ $(OBJECTS_OPT) -lm

# Unoptimized version (-O0)
$(TARGET_J): $(OBJECTS)
	$(CC) $(OPT_NONE) -o $@ $(OBJECTS) -lm

# Threads placement version (-O3)
$(TARGET_J_THREAD): $(OBJECTS_OPT)
	$(CC) $(OPT_FAST) -o $@ $(OBJECTS_OPT) -lm

# Compilation rules for standard (-O0)
%.o: %.c
	$(CC) $(OPT_NONE) -D_JACOBI -D_JACOBI_PARALLEL -D_JACOBI_PARALLEL_OPT -c $< -o $@

# Compilation rules for optimized (-O3)
%_opt.o: %.c
	$(CC) $(OPT_FAST) -D_JACOBI -D_JACOBI_PARALLEL -D_JACOBI_PARALLEL_OPT -c $< -o $@

# Compilation rules for optimized thread placement(-O3)
%_thread.o: %.c
	$(CC) $(OPT_FAST) -D_JACOBI -D_JACOBI_PARALLEL -D_JACOBI_PARALLEL_OPT -c $< -o $@

clean:
	rm -f *.o $(TARGET_J) $(TARGET_J_OPT) $(TARGET_J_THREAD)

realclean: clean
	rm -f jacobi_results.data