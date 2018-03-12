/* Gregory Thornton  September 19,2017  COSC341 Fall-2017*/
#include <stdio.h>
int main() {

	int arr[] = {1, 3, 2, 4, 5, 6, 2};
	int length  = sizeof arr/ sizeof(int);
	int i;

	printf("Array contents: ");

	for(i = 0; i <length; i++){
	printf(" %d ",arr[i]);
	}

	printf("\nEven integers: ");

	for(i = 0; i <length; i++){
		if(arr[i]%2==0){
			printf(" %d ",arr[i]);
		}
	}
	printf("\n");
}
/*
Last login: Tue Sep 19 14:48:20 on ttys001
cs-520-5-4:~ gthornt2$ cd Desktop
cs-520-5-4:Desktop gthornt2$ cc program1a.c
cs-520-5-4:Desktop gthornt2$ ./a.out
Array contents:  1  3  2  4  5  6  2 
Even integers:  2  4  6  2 
cs-520-5-4:Desktop gthornt2$ 
*/