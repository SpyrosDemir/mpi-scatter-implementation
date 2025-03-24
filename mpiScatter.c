#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

#define LENGTH 20

struct min1 {
	float value;
	int index;
} sendmin, recievemin;

struct min2 {
	float value;
	int index;
} sendmin1, recievemin1;

int main(int argc, char** argv) {
    int n, p, my_rank, i, j, flag = 0, j_cord, i_cord;
    float A[LENGTH][LENGTH], B[LENGTH][LENGTH], max;
    MPI_Status status;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    // Input and broadcast size of the array
    if (my_rank == 0) {
        // Prompt user for the number of rows and columns
        do {
            printf("Enter the number of rows and columns (max 20): ");
            scanf("%d", &n);
        } while (n > 20 || n <= 0 || n % 2 != 0 );

        // Input array elements
        printf("Enter the array: \n");
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                scanf("%f", &A[i][j]);
            }
        }
    }
    
    // Broadcast the size of the array to all processors
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
    // Allocate memory for local parts of the array in each processor
    int local_n = n / p;
    float local_A[local_n][LENGTH];
	
    // Scatter the array among all processors
    MPI_Scatter(&(A[0][0]), local_n * LENGTH, MPI_FLOAT, &(local_A[0][0]), local_n * LENGTH, MPI_FLOAT, 0, MPI_COMM_WORLD);

    float local_sum;
    int local_flag = 0;

    // Check for strictly diagonal dominance
    for (i = 0; i < local_n; i++) {
        local_sum = 0.0;
        for (j = 0; j < n; j++) {
            if (j != my_rank * local_n + i) {
                local_sum = local_sum + fabs(local_A[i][j]);
            }
        }
        if (local_sum > fabs(local_A[i][my_rank * local_n + i])) {
            local_flag = 1;
            break;
        }
    }

    MPI_Reduce(&local_flag, &flag, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    // Print result based on diagonal dominance
    if (my_rank == 0) {
        if (flag) {
            printf("No, the array is not diagonally dominant\n");
        } else {
            printf("Yes, the array is diagonally dominant\n");
        }
    }
    
    MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (flag == 0){
        // Find the maximum absolute value on the diagonal locally
        float local_max = fabs(local_A[0][my_rank * local_n]);
        for (i = 0; i < local_n; i++) {
            for (j = 0; j < n; j++) {
                if (j == my_rank * local_n + i) {
                    if (fabs(local_A[i][j]) > local_max) {
                        local_max = fabs(local_A[i][j]);
                    }
                }
            }
        }
        
        // Reduce local maximums to find the overall maximum of the diagonal elements
        MPI_Reduce(&local_max, &max, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
        
        // Process 0 prints the absolute maximum of the diagonal and broadcasts it
        if (my_rank == 0) {
            printf("The max of the diagonal is %f\n", max);
        }
        MPI_Bcast(&max, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    	
    	
    	float local_B[local_n][LENGTH];
    	
		// Create the B array with modified values
		for (i = 0; i < local_n; i++) {
			for (j = 0; j < n; j++) {
				if (j != my_rank * local_n + i) {
					local_B[i][j] = max - fabs(local_A[i][j]);
				}
				else {
					local_B[i][j] = max;
				}
			}
		}

		// Gather modified local_B arrays into the global B array
		MPI_Gather(&local_B[0][0], local_n * LENGTH, MPI_FLOAT, &B[0][0], local_n * LENGTH, MPI_FLOAT, 0, MPI_COMM_WORLD);
		
		if(my_rank==0){
			// Print the global B array
			printf("The B array is: ");
			for(i=0; i < n; i++){
				printf("\n");
				for(j=0; j < n; j++){
					printf("%.2f ",B[i][j]);
				}
			}	
		}
		
	    // Find minimum and coords in the modified array
	    sendmin.value = local_B[0][0];
	    sendmin1.value = local_B[0][0];
        for (i = 0; i < local_n; i++) {
            for (j = 0; j < n; j++) {
                if (local_B[i][j] < sendmin.value) {
                    sendmin.value = local_B[i][j];
                    sendmin1.value = local_B[i][j];
                    sendmin.index = i +1 + (my_rank*local_n);
                    sendmin1.index = j+1;
                }
            }
        }

        MPI_Reduce(&sendmin, &recievemin, 1, MPI_2INT, MPI_MINLOC, 0, MPI_COMM_WORLD);
        MPI_Reduce(&sendmin1, &recievemin1, 1, MPI_2INT, MPI_MINLOC, 0, MPI_COMM_WORLD);


        if (my_rank == 0) {

            printf("\nThe minimum of B is %f and it's at position %d, %d \n", recievemin.value, recievemin.index, recievemin1.index);
        }
    }
	
    MPI_Finalize();

    return 0;
}
