/* Gregory Thornton  September 19,2017  COSC341 Fall-2017*/
#include <stdio.h>
#include <stdlib.h>

int main(){
	int first, second, third, median;

	printf("Input three integers\n");

	scanf("%d %d %d", &first, &second, &third);
	
	printf("Entered value: %d\n", first);
	printf("Entered value: %d\n", second);
	printf("Entered value: %d\n\n", third);
	
	if((first > second && first < third) || (first < second && first > third)){
		median = first;
	} else if((second > first && second < third) || (second < first && second > third)){
		median = second;
	} else{
		median = third;
	}
	
	printf("The median is: %d\n", median );
	
}
/*
cs-520-5-4:Desktop gthornt2$ cc program1c.c
cs-520-5-4:Desktop gthornt2$ ./a.out
Input three integers
892 353 1245
Entered value: 892
Entered value: 353
Entered value: 1245

The median is: 892
cs-520-5-4:Desktop gthornt2$ 
*/