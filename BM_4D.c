// This C code is for generating 4-dimensional partitions by implementing Bratley-McKay algorithm.
// Author: Avinandan Mondal.
// Code version: Rolling version
/* Current version description:
It stores lattice points as uint32_t integers with most significant 2 bits unallocated (set to 0 always)
(6 bits for each coordinate * 5 coordinates) + 2 unallocated bits = 32 bits.
It stores the current partition as a boolean array over the lattice points.
Lattice points are used as indices of the partition array "part".*/


/* IMPORTANT COMPILATION INSTRUCTIONS:
For generating fastest compiled code, use appropriate compiler flags.
For example, for gcc compiler, use the flag:
-O3 -march=native -funroll-loops
*/


# include <stdio.h>
# include <stdbool.h>
# include <stdlib.h>
# include <string.h>
# include <stdint.h>
# include <time.h> // For benchmarking purposes



// NOTE THAT THIS CODE WORKS ONLY TILL n=63.

#define N 0x40000000
#define M 300

static bool part[N];
static uint32_t poss[M];

static const size_t SIZE = sizeof(uint32_t);

static const uint32_t FIELD_MASK[5] = {
                                0x3Fu << 24,
				0x3Fu << 18,
				0x3Fu << 12,
				0x3Fu << 6,
				0x3Fu
                                };

static const uint32_t FIELD_OP[5] = {
                                1u << 25,
				1u << 19,
				1u << 13,
				1u << 7,
				1u << 1
                                };



uint32_t part_counter;
uint32_t poss_len;


bool ispossible(uint32_t idx){
	for (int j=0; j<5; j++){
		if (idx & FIELD_MASK[j]){
			if (!part[idx - FIELD_OP[j]]){
				return 0;
			}
		}
	}
	return 1;
}


void new_node_finder(uint32_t new_idx[5], uint32_t hot_idx, uint32_t *num_new_ptr){
	uint32_t count = 0;
	for (int j=0; j<5; j++){
		hot_idx += FIELD_OP[j];
		if (ispossible(hot_idx)){
			new_idx[count] = hot_idx;
			count ++;
		}
		hot_idx -= FIELD_OP[j];
	}
	*num_new_ptr = count;
}


void part_adder(int init, int fin, uint32_t n, uint64_t *arr){
	arr[part_counter - 1] += 1;
	if (part_counter == n){
		return;
	}
	else{
		for (int j=init; j<=fin; j++){
			uint32_t hot_idx = poss[j];
			part[hot_idx] ^= 1; // 0 ----> 1
			part_counter += 1;
			uint32_t new_idx[5];
			uint32_t num_new;
			uint32_t *num_new_ptr = &num_new;
			new_node_finder(new_idx, hot_idx, num_new_ptr);
			for (int k=0; k<num_new; k++){poss[poss_len + k] = new_idx[k];}
			poss_len += num_new;
			part_adder(j+1, fin+num_new, n, arr);
			part_counter -= 1;
			part[hot_idx] ^= 1; // 1 ----> 0
			poss_len -= num_new;
		}
	}
}

int main(){
	uint32_t n;
	printf("Enter n\n");
	if (scanf("%u", &n) != 1) {
    		fprintf(stderr, "Invalid input\n");
    		return 0;
	}


	// Main computation area + Benchmarking
	struct timespec start, end;
	double total_time;
	clock_gettime(CLOCK_MONOTONIC, &start);

	uint64_t arr[n];
	memset(arr, 0, n * sizeof *arr);
	uint32_t hot_idx = 0x00000000;
	part[hot_idx] = 1;
	part_counter = 1;
	uint32_t new_idx[5];
	uint32_t num_new;
	uint32_t *num_new_ptr = &num_new;
	new_node_finder(new_idx, hot_idx, num_new_ptr);
	memcpy(poss, new_idx, (size_t)num_new*SIZE);
	poss_len = num_new;
	part_adder(0, num_new-1, n, arr);


	clock_gettime(CLOCK_MONOTONIC, &end);
	total_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;


	// Print the results
	printf("\n-----FOUR-DIM PARTITIONS ENUMERATION-----\n");
	for(int j = 0; j<n; j++){
		printf("%u \t %lu\n", j+1, arr[j]);
	}
	printf("----END OF ENUMERATION----\n");
	printf("Total computation time needed: %f seconds\n", total_time);


	return 1;
}
