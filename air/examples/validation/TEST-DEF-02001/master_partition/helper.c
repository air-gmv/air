/*
 * helper.c
 *
 *  Created on: Jun 29, 2012
 *      Author: cassia
 */

#include <string.h>
#include <pal_printf.h>

/* returns input length
 *
 * */
int nb_char(long long i){

	if(i < 0){
		printf("< out of range\n");
		return 1;
	}else if(i > 999999999){//1000000000
		return 10;
	}else if(i > 99999999){
		return 9;
	}else if(i > 9999999){
		return 8;
	}else if(i > 999999){
		return 7;
	}else if(i > 99999){
		return 6;
	}else if(i > 9999){
		return 5;
	}else if(i > 999){
		return 4;
	}else if(i > 99){
		return 3;
	}else if(i > 9){
		return 2;
	}else return 1;

}


/*
 * return the char equivalent to the input integer
 * */
char to_char(long long i, long long base, int *o_cint){

	char numbers[10] = {'0','1','2','3','4','5','6','7','8','9'};
	int c = 0;
	int new_i = 0;
	*o_cint = 0;

	if(i < base){
		*o_cint = 0;
		return numbers[0];
	}

	if(i < 0){
		printf("< out of range\n");
		return 'a';
	}

	if(i > 9999999999){
		printf("> out of range\n");
		return 'a';
	}

	else
	if(i > 999999999){
		c = i / 1000000000;
		new_i = i - (c * 100000000);
	}else
	if(i > 99999999){
		c = i / 100000000;
		new_i = i - (c * 10000000);
	}else if(i > 9999999){
		c = i / 10000000;
		new_i = i - (c * 10000000);
	}else if(i > 999999){
		c = i / 1000000;
		new_i = i - (c * 1000000);
	}else if(i > 99999){
		c = i / 100000;
		new_i = i - (c * 100000);
	}else if(i > 9999){//10000
		c = i / 10000;
		new_i = i - (c * 10000);
	}else if(i > 999){ //1000
		c = i / 1000;
		new_i = i - (c * 1000);
	}else if(i > 99){ //100
		c = i / 100;
		new_i = i - (c * 100);
	}else if(i > 9){
		c = i / 10;
		new_i = i - (c * 10);
	}else{
		*o_cint = i;
		return numbers[i];
	}

	*o_cint = c;
	return numbers[c];
}

/* 10 ^ i_nb */
long decimals(int i_nb){
	int i = 0;
	long result = 1;

	for(i = 1; i < i_nb; i++){
		result = result * 10;
	}
	return result;
}



int int_to_char(long long i_input, char * o_output){

	long long input = i_input;
	char output[20];
	char c;
	int input_len = 0;
	int i = 0;
	long long base = 0;
	int c_int = 0;

	memset(output, 0, 20);

	input_len = nb_char(input);

	for(i = 0; i < input_len; i++){
		base = decimals(input_len - i);
		c = to_char(input, base, &c_int);
		output[i] = c;
		strcat(output, "\0");
		input = input - (c_int * base);

	}

	//printf("* %s\n", output);

	strcpy(o_output, output);

	return 0;

}
