#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

typedef enum lexeme_t
{
	NUMBER,
	LETTER,
	CARAC,
	BEGIN
} lexeme_t;

typedef struct lexeme
{
	lexeme_t type;
	char valeur[20];
} lexeme;


typedef struct CharList
{
	int size;
	char* content;
} CharList;

CharList* remove_comment(FILE* file){
	CharList* res = malloc(sizeof(CharList));
	res->size = 0;
	res->content = malloc(sizeof(char)*1000);
	int curr;
	int num = 0;
	char comments[100];
	
	// fgetc() return int
	int prev = fgetc(file);
	while((curr = fgetc(file)) != EOF){
		if(prev == '/'){
			if(curr == '/'){
				fgets(comments, 100, file);
				prev = '\n';
				continue;
			}
			else if(curr == '*'){
				prev = fgetc(file);
				while((curr = fgetc(file)) != EOF){
					if(prev == '*' && curr == '/'){
						prev = '\n';
						break;
					}
					else prev = curr;
				}
			}
			else{
				res->content[num] = prev;
				num++;
				prev = curr;
			}
		}
		else{
			res->content[num++] = prev;
			prev = curr;
		}
		
	}
	
	res->content[num++] = prev;
	res->size = num;
	return res;
}

int is_letter(char a){
	if((a>='A' && a<='Z') || (a>='a' && a<='z')) return 1;
	else return 0;
}

int is_number(char a){
	if(a>='0' && a<='9') return 1;
	else return 0;
}

int is_symbol(char a){
	if(a=='(' || a==')' || a=='[' || a==']' || a=='{' || a=='}' || a=='=' || a=='"' || a=='`' || a==',' || a=='\n' || a==' ' || a=='\t') return 1;
	else return 0;
}

int is_paren_left(char a){
	if(a=='(' || a=='[' || a=='{') return 1;
	else return 0;
}

int is_paren_right(char a){
	if(a==')' || a==']' || a=='}') return 1;
	return 0;
}

int is_chinese(CharList* l, int index){
	char special_matrix[12][3] = {{-23,-101,-74},{-28,-72,-128},{-28,-70,-116},{-28,-72,-119},{-27,-101,-101},{-28,-70,-108},{-27,-123,-83},{-28,-72,-125},{-27,-123,-85},{-28,-71,-99},{-27,-115,-127},{-30,-122,-110}};
	char a1 = l->content[index];
	char a2 = l->content[index+1];
	char a3 = l->content[index+2];
	
	for(int i=0; i<12; i++){
		if(a1==special_matrix[i][0] && a2==special_matrix[i][1] && a3==special_matrix[i][2]) return 1;
	}
	return 0;
}

int character_correct(CharList* l){
	lexeme_t type = BEGIN;
	char a;
	int paren_s = 0, paren_m = 0, paren_l = 0;

	for(int i=0; i<l->size; i++){
		a = l->content[i];
		if(type == BEGIN){
			if(is_letter(a)) type = LETTER;
			else if(is_number(a)) type = NUMBER;
			else if(is_symbol(a)){
				if(is_paren_left(a)){
					if(a == '(') paren_s++;
					else if(a == '[') paren_m++;
					else paren_l++;
			}
				else if(is_paren_right(a)){
					if(a == ')') paren_s--;
					else if(a == ']') paren_m--;
					else paren_l--;
			}
			
			}
			else if(is_chinese(l, i)){
				type = CARAC;
				i += 2;
			}
			else{
				if(a < 0){
					char ch1 = l->content[i];
					char ch2 = l->content[i+1];
					char ch3 = l->content[i+2];
					i += 2;
					printf("Error: Unable to recognize character: %c", ch1);
					printf("%c", ch2);
					printf("%c\n", ch3);
					return 1;
				}
				else{
					printf("Error: Unable to recognize character: %c\n", a);
					printf("%d\n", a);
					return 1;
				}
			}
		}
		else if(type == LETTER){
			if(!is_letter(a)){
				type = BEGIN;
				i--;
			}
		}
		else if(type == NUMBER){
			if(!is_number(a)){
				type = BEGIN;
				i--;
			}
		}
		else if(type == CARAC){
			if(is_chinese(l, i)) i += 2;
			else{
				type = BEGIN;
				i--;
			}
		}
	}
	if(paren_s > 0){
		printf("Error: ')' is missing.\n");
		return 2;
	}
	if(paren_m > 0){
		printf("Error: ']' is missing.\n");
		return 2;
	}
	if(paren_l > 0){
		printf("Error: '}' is missing.\n");
		return 2;
	}
	if(paren_s < 0){
		printf("Error: '(' is missing.\n");
		return 2;
	}
	if(paren_m < 0){
		printf("Error: '[' is missing.\n");
		return 2;
	}
	if(paren_l < 0){
		printf("Error: '{' is missing.\n");
		return 2;
	}

	return 0;
}

int word_correct(CharList* l){
	char motcle_1[] = "Automate";
    char motcle_2[] = "etats";
    char motcle_3[] = "initial";
    char motcle_4[] = "final";
    char motcle_5[] = "transitions";

	char word[50][50] = {0};
	int row = 0, col = 0;
	char a;
	int match;
	for(int i=0; i<l->size; i++){
		a = l->content[i];
		
		if(a == '`'){
			while(a == '`') a = l->content[i++];
			while(a != '`') a = l->content[i++];
			while(a == '`') a = l->content[i++];
			i--;
		}

		if(a == '"'){
			while(a == '"') a = l->content[i++];
			while(a != '"') a = l->content[i++];
			while(a == '"') a = l->content[i++];
			i--;
		}

		while(is_letter(a)){
			word[row][col] = a;
			col++;
			if(!is_letter(l->content[i+1])){
				row++;
				col = 0;
				break;
			}
			i++;
			a = l->content[i];
		}
	}

	for(int i=0; i<row; i++){
		if(word[i][0] == 'A'){
			match = strcmp(word[i], motcle_1);
			if(match != 0){
				printf("Error: can't recognize the word: %s\n", word[i]);
				return 1;
			}
		}
		else if(word[i][0] == 'e'){
			match = strcmp(word[i], motcle_2);
			if(match != 0){
				printf("Error: can't recognize the word: %s\n", word[i]);
				return 1;
			}
		}
		else if(word[i][0] == 'i'){
			match = strcmp(word[i], motcle_3);
			if(match != 0){
				printf("Error: can't recognize the word: %s\n", word[i]);
				return 1;
			}
		}
		else if(word[i][0] == 'f'){
			match = strcmp(word[i], motcle_4);
			if(match != 0){
				printf("Error: can't recognize the word: %s\n", word[i]);
				return 1;
			}
		}
		else if(word[i][0] == 't'){
			match = strcmp(word[i], motcle_5);
			if(match != 0){
				printf("Error: can't recognize the word: %s\n", word[i]);
				return 1;
			}
		}
		else{
			printf("Error: can't recognize the word: %s\n", word[i]);
			return 1;
		}
	}
	return 0;
}

int main(void)
{
	FILE *fp = NULL;

	fp = fopen("./automates/Zpile.txt", "r");
	
	if(fp == NULL)
	{
		printf("CAN NOT LOAD TXT FILE\n");
		return 1;
	}
	
	CharList* res = remove_comment(fp);
	printf("%d\n", res->size);
	
	for(int i=0; i<res->size; i++){
		printf("%c", res->content[i]);
	}
	
	fclose(fp);

	FILE* fp_out = fopen("test_resultat_lexical.txt", "w");
	char a;
	for(int i=0; i<res->size; i++){
        a = res->content[i];
        if(a != ' ' && a!='\n' && a!='\t') fputc(a, fp_out);
	}

	int error_character = character_correct(res);
	if(error_character == 0) printf("Characters are correct.\n");
	else printf("Error in characters.\n");

	int error_word = word_correct(res);
	if(error_word) printf("Error in word.\n");
	else printf("Words are correct.\n");

	return 0;
}
