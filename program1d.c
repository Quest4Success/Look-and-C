/* Gregory Thornton  September 19,2017  COSC341 Fall-2017*/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	
	int size = argc-2;
	int arr[size];
	int i;
	int median;
	
	
	printf("%d\n", size);
	for (i =1; i<argc; i++)
	{
		sscanf(argv[i], "%d", &arr[i-1]);
	}

	printf("Entered value: %d\n", arr[0]);
	printf("Entered value: %d\n", arr[1]);
	printf("Entered value: %d\n", arr[2]);
	
	if((arr[0]>arr[1] && arr[0]<arr[2]) || (arr[0]<arr[1] && arr[0]>arr[2])){
		median = arr[0];
	} else if((arr[1]>arr[0] && arr[1]<arr[2]) || (arr[1]<arr[0] && arr[1]>arr[2])){
		median = arr[1];
	} else{
		median = arr[2];
	}

	printf("\nThe median is: %d\n", median );

}
/*
cs-520-5-4:Desktop gthornt2$ cc program1d.c
cs-520-5-4:Desktop gthornt2$ ./a.out 464 256 23335
Entered value: 464
Entered value: 256
Entered value: 23335

The median is: 464
cs-520-5-4:Desktop gthornt2$ 
*/