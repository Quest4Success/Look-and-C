/* Gregory Thornton  September 19,2017  COSC341 Fall-2017*/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	int size = argc-1;
	int arr[size];
	int i;
	int even;
	
	for (i =1; i<argc; i++)
	{
		sscanf(argv[i], "%d", &arr[i-1]);
		if(arr[i-1]%2==0){
			even++;
		}
	}

	printf("Entered Integers:");

	for(i = 0; i <size; i++){
		printf(" %d ", arr[i]);
	}
	printf("\nNumber of even integers: %d\n", even );
}