// N-Queen (Using Multithreading)
// Version 3.0

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

int size;
FILE *ptr[20];
int Output_in_File = 0; // to see output in file (change value to 1)

void initializeBoard(int size, int **board);
void displayDottedLine();
void displayBoard(int size, int **board);
void displayMatrix(int size, int **board);
bool isLocationAvailable(int row, int col, int size, int **board);
void solveNQueens(int col, int size, int **board, int threadNumber, int *count, FILE **ptr);
void *N_Queen_Start(void *args);

void initializeBoard(int size, int **board)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            board[i][j] = 0; // Initialize board with empty positions
        }
    }
}

void displayMatrix(int size, int **board)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%d ", board[i][j]); // Empty position
        }
        printf("\n");
    }
    printf("\n");
}

void displayBoard(int size, int **board)
{

    printf("\n");
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (board[i][j] == 1)
            {
                printf("Q  "); // Queen placed
            }
            else
            {
                printf("-  "); // Empty position
            }
        }
        printf("\n");
    }
    printf("\n");
}

bool isLocationAvailable(int row, int col, int size, int **board)
{
    // Check if no queen is present in the same col
    for (int i = 0; i < row; i++)
    {
        if (board[i][col] == 1)
        {
            return false;
        }
    }

    // Check upper diagonal on left side
    for (int i = row, j = col; i >= 0 && j >= 0; i--, j--)
    {
        if (board[i][j] == 1)
        {
            return false;
        }
    }

    // Check upper diagonal on right side
    for (int i = row, j = col; i >= 0 && j < size; i--, j++)
    {
        if (board[i][j] == 1)
        {
            return false;
        }
    }
    return true;
}

int *get_solution_array(int **board, int size)
{
    int *solution_array = (int *)malloc(size * sizeof(int));

    int k = 0;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (board[i][j] == 1)
            {
                solution_array[k] = j;
                k++;
            }
        }
    }
    return solution_array;
}

void solveNQueens(int row, int size, int **board, int threadNumber, int *count, FILE **ptr)
{
    if (row == size)
    {
        (*count)++;

        if (Output_in_File == 1)
        {
            int *solution_array = get_solution_array(board, size);

            // writing in file
            for (int i = 0; i < size; i++)
            {
                fprintf(ptr[threadNumber - 1], "%d ", solution_array[i]);
            }
            fprintf(ptr[threadNumber - 1], "\n");
            free(solution_array); // Free allocated memory to solution array
        }
        return;
    }

    for (int j = 0; j < size; j++)
    {
        if (isLocationAvailable(row, j, size, board))
        {

            board[row][j] = 1; // Queen placed successfully
            solveNQueens(row + 1, size, board, threadNumber, count, ptr);

            // Backtrack: remove queen from this position
            board[row][j] = 0;
        }
    }
}

void *N_Queen_Start(void *args)
{
    int *threadArgs = (int *)args;
    int threadNumber = threadArgs[0];
    int *localCount = &threadArgs[1]; // every thread have their own count

    // initializing 2-d array
    int **board = (int **)malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++)
    {
        board[i] = (int *)malloc(size * sizeof(int));
    }

    // filling board with 0
    initializeBoard(size, board);
    board[0][threadNumber - 1] = 1;

    if (Output_in_File == 1)
    {
        char filename[20];
        snprintf(filename, sizeof(filename), "solution_%d.txt", threadNumber);
        ptr[threadNumber - 1] = fopen(filename, "a");
    }

    solveNQueens(1, size, board, threadNumber, localCount, ptr);

    if (Output_in_File == 1)
    {
        fclose(ptr[threadNumber - 1]); // Close file
    }
    for (int i = 0; i < size; i++)
    {
        free(board[i]);
    }
    free(board);

    printf("\nFrom Thread %d: local count : %d", threadNumber, *localCount);
}

void displayDottedLine()
{
    for (int i = 1; i <= 30; i++)
    {
        printf("--");
    }
}

void deleteTxtFiles()
{
    system("rm -f *.txt"); // Remove all .txt files in the cwd
    system("rm -f *.tx");  // Remove all .tx files in the cwd
}

int main()
{
    int counter = 0; // Total solutions
    printf("\n");
    deleteTxtFiles();
    struct timespec ts_start, ts_end;
    double time;
    displayDottedLine();
    printf("\nMulti-Threaded N-Queen Program\n");
    displayDottedLine();
    printf("\nEnter Order : ");
    scanf("%d", &size);
    displayDottedLine();

    pthread_t threads[size];
    int threadArgs[size][2];

    if (size <= 0 || size == 2 || size == 3)
    {
        printf("\nSolutions not possible...");
        return 1;
    }
    else if (size == 1)
    {
        printf("Total number of solutions: 1");
        return 1;
    }

    for (int i = 0; i < size; i++)
    {
        threadArgs[i][0] = i + 1; // Thread number
        threadArgs[i][1] = 0;     // Initialize local count to 0
        pthread_create(&threads[i], NULL, N_Queen_Start, (void *)&threadArgs[i]);
    }

    clock_gettime(CLOCK_MONOTONIC, &ts_start);
    for (int i = 0; i < size; i++)
    {
        pthread_join(threads[i], NULL);
        counter += threadArgs[i][1];
    }
    clock_gettime(CLOCK_MONOTONIC, &ts_end);

    printf("\n\nTotal number of solutions: %d\n", counter);

    long long int ns_start = ts_start.tv_sec * 1000000000LL + ts_start.tv_nsec;
    long long int ns_end = ts_end.tv_sec * 1000000000LL + ts_end.tv_nsec;
    long long int executionTime = ns_end - ns_start;
    printf("\nExecution Time : %lf seconds\n", (double)executionTime / 1000000000);
    displayDottedLine();
    printf("\n\n");
    return 0;
}

// for optimization (compile using this)
// gcc -O3 -march=native N_Queen_Threaded.c