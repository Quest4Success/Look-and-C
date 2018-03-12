/* Gregory Thornton
	Assignment 2
	COSC 341 September 26, 2017
	URL: https://people.emich.edu/gthornt2/COSC341/program2b.c*/
#include <stdio.h>
#include <stdlib.h>
	int main(int argc, char* argv[]){
	
		int arr[100];
		int i;
		int r;
		int low;
		int temp;

		for (i = 1; i<argc; i++)
		{
			sscanf(argv[i], "%d", &arr[i-1]);
		}

		printf(" Contents in array: ");

		for(i = 0; i < argc-1; i++){
			printf(" %d ", arr[i]);
		}

		printf("\nContent in order: ");

		for(i = 0; i < argc-1; i++){
			low = i;
			r =i+1;
			while(r<argc-1){
				if(arr[r]<arr[low]){
					low = r;
				}
				r++;
			}

			temp = arr[i];
			arr[i] = arr[low];
			arr[low] = temp;
		}

		for(i = 0; i < argc-1; i++){
			printf(" %d ", arr[i]);
		}
		printf("\n");
		return(0);
	}  
	
	/* Last login: Tue Sep 26 14:02:53 on ttys000
cs-520-5-4:~ gthornt2$ cd Desktop
cs-520-5-4:Desktop gthornt2$ cc program2b.c
cs-520-5-4:Desktop gthornt2$ ./a.out 19 17 13 11 7 5 3 2 1 31 29 23
 Contents in array:  19  17  13  11  7  5  3  2  1  31  29  23 
Content in order:  1  2  3  5  7  11  13  17  19  23  29  31 
cs-520-5-4:Desktop gthornt2$ 
*/