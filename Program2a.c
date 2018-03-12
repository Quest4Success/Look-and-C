/* Gregory Thornton
	Assignment 2
	COSC 341 September 26, 2017
	URL: https://people.emich.edu/gthornt2/COSC341/Program2a.c */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
	int main(){

		int size=10;
		int arr[size];
		int i;
		int r;

		time_t t;
		srand(time(&t));

		for(i = 0; i < size; i++){
			arr[i] = i;
		}

		printf(" Contents in array: ");

		for(i = 0; i < size; i++){
			printf(" %d ", arr[i]);
		}

		printf(" \n5 random numbers: ");

		for(i = 0; i < 5; i++){
			r = rand()%size;
			printf(" %d ",arr[r]);
			arr[r] = arr[size-1];
		}
		printf("\n");
		return(0);
	}
	/* Last login: Tue Sep 26 13:53:42 on ttys000
cs-520-5-4:~ gthornt2$ cd Desktop
cs-520-5-4:Desktop gthornt2$ cc program2a.c
cs-520-5-4:Desktop gthornt2$ ./a.out
 Contenets in array:  0  1  2  3  4  5  6  7  8  9  
5 random numbers:  2  6  4  1  3 
cs-520-5-4:Desktop gthornt2$ ./a.out
 Contenets in array:  0  1  2  3  4  5  6  7  8  9  
5 random numbers:  3  6  9  8  9 
cs-520-5-4:Desktop gthornt2$ ./a.out
 Contenets in array:  0  1  2  3  4  5  6  7  8  9  
5 random numbers:  0  5  9  9  9 
cs-520-5-4:Desktop gthornt2$ ./a.out
 Contenets in array:  0  1  2  3  4  5  6  7  8  9  
5 random numbers:  7  4  1  9  2 
cs-520-5-4:Desktop gthornt2$ 
*/