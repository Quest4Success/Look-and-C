/* Gregory Thornton  September 19,2017  COSC341 Fall-2017*/
#include <stdio.h>
int main() {

	int arr[] = {1, 3, 2, 4, 5, 6, 2};
	int length  = sizeof arr/ sizeof(int);
	int i;
	int sum=0;

	printf("Array contents: ");

	for(i = 0; i <length; i++){
	printf(" %d ",arr[i]);
	}

	for(i = 0; i <length; i++){
		if(i%2==0){
			sum+=arr[i];
		}
	}

	printf("\nSum: %d", sum);
	printf("\nThis is: ");

	for(i = 0; i <length; i++){
		if(i%2==0){
			printf(" %d ", arr[i]);
		}
	}
	printf("\n");
}
/*
Last login: Tue Sep 19 15:15:31 on ttys000
cs-520-5-4:~ gthornt2$ cd Desktop
cs-520-5-4:Desktop gthornt2$ cc program1b.c
cs-520-5-4:Desktop gthornt2$ ./a.out
Array contents:  1  3  2  4  5  6  2 
Sum: 10
This is:  1  2  5  2 
cs-520-5-4:Desktop gthornt2$ 
*/