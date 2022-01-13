#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

int etat;

typedef struct CharList
{
	int size;
	char* content;
} CharList;

CharList* read_file(){
    FILE* fp = NULL;
    fp = fopen("test_resultat_lexical.txt", "r");

    if(fp == NULL){
        printf("Can't open the resultat_lexical.txt\n");
        return NULL;
    }
    
    char a;
    CharList* res = malloc(sizeof(CharList));
	res->size = 0;
	res->content = malloc(sizeof(char)*1000);
    while((a = fgetc(fp))!=EOF){
        res->content[res->size] = a;
        res->size++;
    }

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

int is_chinese(CharList* l, int index){
	char chinese_matrix[11][3] = {{-23,-101,-74},{-28,-72,-128},{-28,-70,-116},{-28,-72,-119},{-27,-101,-101},{-28,-70,-108},{-27,-123,-83},{-28,-72,-125},{-27,-123,-85},{-28,-71,-99},{-27,-115,-127}};
	char a1 = l->content[index];
	char a2 = l->content[index+1];
	char a3 = l->content[index+2];
	
	for(int i=0; i<11; i++){
		if(a1==chinese_matrix[i][0] && a2==chinese_matrix[i][1] && a3==chinese_matrix[i][2]) return 1;
	}
	return 0;
}

int analyse_syntaxique(CharList* l){
    etat = 0;
    FILE* output;
    output = fopen("test_resultat_syntaxique.txt", "w");
    if(output == NULL){
       printf("can't create the resultat_syntaxique.txt\n");
       return 1;
    }
    
    char a;
    int flag = 0;
    for(int i=0; i<l->size; i++){
        a = l->content[i];
        if(etat == 0){
            if(a == 'A'){
                i += 7;
                etat = 1;
            }
            else{
                fclose(output);
                printf("Can't find the key word: Automate\n");
                return 1;
            }
        }
        else if(etat == 1){
            if(a == '(') etat++;
            else{
                fclose(output);
                printf("'=' misses after Automate.\n");
                return 1;
            }
        }
        else if(etat == 2){
            if(is_number(a)){
                etat++;
                fputs("pile: ", output);
                fputc(a, output);
                fputs(";\n", output);
            }
            else{
                fclose(output);
                printf("The number of stacks misses.\n");
                return 1;
            }
        }
        else if(etat == 3){
            if(a == ')') etat++;
            else{
                fclose(output);
                printf("')' misses at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 4){
            if(a == '=') etat++;
            else{
                fclose(output);
                printf("'=' misses at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 5){
            if(a == '{') etat++;
            else{
                fclose(output);
                printf("'{' misses at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 6){
            if(a == 'e'){
                etat++;
                i += 4;
                fputs("etats: ", output);
            } 
            else{
                fclose(output);
                printf("'etats' misses at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 7){
            if(a == '=') etat++;
            else{
                fclose(output);
                printf("'=' misses at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 8){
            if(a == '[') etat++;
            else{
                fclose(output);
                printf("'[' misses at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 9){
            if(a == ']'){
                etat = 12;
                fputs(";\n", output);
            } 
            else if(a == '"'){
                fputc('"', output);
                flag = 1;
                etat++;
            }
            else if(a == '`'){
                fputc('"', output);
                flag = 2;
                etat++;
            }
            else{
                fclose(output);
                printf("can't read 'etats' at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 10){
            if(is_number(a) || is_letter(a)){
                etat = 10;
                fputc(a, output);
            }
            else if(is_chinese(l, i)){
                fputc(l->content[i], output);
                fputc(l->content[i+1], output);
                fputc(l->content[i+2], output);
                i += 2;
                etat = 10;
            }
            else if(a == '"'){
                if(flag == 1){
                    etat++;
                    fputc('"', output);
                }
                else{
                    fclose(output);
                    printf("'\"' misses at the %dth place.\n", i);
                    return 1;
                }
            }
            else if(a == '`'){
                if(flag == 2){
                    etat++;
                    fputc('"', output);
                }
                else{
                    fclose(output);
                    printf("'`' misses at the %dth place.\n", i);
                    return 1;
                }
            }
            else{
                fclose(output);
                printf("can't read 'etats' at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 11){
            if(a == ',') etat = 9;
            else if(a == ']'){
                etat = 12;
                fputs(";\n", output);
            } 
            else{
                fclose(output);
                printf("',' or ']' misses at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 12){
            if(a == 'i'){
                i += 6;
                etat++;
                fputs("initial: ", output);
            }
            else{
                fclose(output);
                printf("can't read 'initial' at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 13){
            if(a == '='){
                etat++;
                fputc('"', output);
            }
            else{
                fclose(output);
                printf("'=' misses at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 14){
            if(is_number(a)){
                fputc(a, output);
                etat++;
            }
            else{
                fclose(output);
                printf("can't read 'initial' at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 15){
            if(is_number(a)){
                fputc(a, output);
            }
            else if(a == 'f'){
                fputs("\";\n", output);
                etat++;
                i += 4;
                fputs("final: ", output);
            }
            else{
                fclose(output);
                printf("can't read 'initial' and 'final' at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 16){
            if(a == '=') etat++;
            else{
                fclose(output);
                printf("'=' misses at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 17){
            if(a == '[') etat++;
            else{
                fclose(output);
                printf("'[' misses at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 18){
            if(is_number(a)){
                fputc('"', output);
                fputc(a, output);
                etat++;
            }
            else{
                fclose(output);
                printf("can't find the final state at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 19){
            if(is_number(a)){
                fputc(a, output);
            }
            else if(a == ','){
                fputc('"', output);
                etat = 18;
            }
            else if(a == ']'){
                fputs("\";\n", output);
                etat++;
            }
            else{
                fclose(output);
                printf("can't read the final state at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 20){
            if(a == 't'){
                etat++;
                i += 10;
                fputs("transitions: ", output);
            }
            else{
                fclose(output);
                printf("can't find 'transitions' at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 21){
            if(a == '=') etat++;
            else{
                fclose(output);
                printf("'=' misses at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 22){
            if(a == '[') etat++;
            else{
                fclose(output);
                printf("'[' misses at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 23){
            if(a == '(') etat++;
            else if(a == ']') etat = 48;
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 24){
            if(is_number(a)){
                fputs("\"(",output);
                fputc(a, output);
                etat++;
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 25){
            if(is_number(a)) fputc(a, output);
            else if(a == -30){
                char b = l->content[i+1];
                char c = l->content[i+2];
                i += 2;
                if(b==-122 && c==-110){
                    fputc(a, output);
                    fputc(b, output);
                    fputc(c, output);
                    etat++;
                }
                else{
                    fclose(output);
                    printf("can't read the transitions at the %dth place.\n", i);
                    return 1;
                }
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 26){
            if(is_number(a)){
                fputc(a, output);
                etat++;
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 27){
            if(is_number(a)) fputc(a, output);
            else if(a == ','){
                fputc(a, output);
                etat++;
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 28){
            if(a == '`'){
                fputc(a, output);
                etat++;
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 29){
            if(is_number(a) || is_letter(a)){
                etat++;
                fputc(a, output);
            }
            else if(a == '`'){
                etat = 31;
                fputc(a, output);
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 30){
            if(a == '`'){
                etat++;
                fputc(a, output);
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 31){
            if(a == ','){
                etat = 34;        // avoir des piles
                fputc(a, output);
            }
            else if(a == ')'){    // pas d'action des piles
                etat++;
                fputc(a, output);
                fputc('"', output);
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 32){
            if(a == ',') etat++;
            else if(a == ']') etat = 48;     // fin de transition
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 33){
            if(a == '(') etat = 24;
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 34){
            if(a == '('){
                etat++;
                fputc(a, output);
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        // 35-40 pour verifier la forme (`a`, ->)
        else if(etat == 35){
            if(a == ')'){
                etat = 45;
                fputc(a, output);
            }
            else if(a == '`'){
                etat++;
                fputc(a, output);
            }
            else if(a == -30){    // fleche
                char b = l->content[i+1];
                char c = l->content[i+2];
                i += 2;
                if(b == -122 && c == -110){
                    etat = 41;
                    fputc(a, output);
                    fputc(b, output);
                    fputc(c, output);
                }
                else{
                    fclose(output);
                    printf("can't read the transitions at the %dth place.\n", i);
                    return 1;
                }
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 36){
            if(is_letter(a) || is_number(a)){
                etat++;
                fputc(a, output);
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 37){
            if(a == '`'){
                etat++;
                fputc(a, output);
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 38){
            if(a == ','){
                etat++;
                fputc(a, output);
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 39){
            if(a == -30){
                char b = l->content[i+1];
                char c = l->content[i+2];
                i += 2;
                if(b == -122 && c == -110){
                    etat = 40;
                    fputc(a, output);
                    fputc(b, output);
                    fputc(c, output);
                }
                else{
                    fclose(output);
                    printf("can't read the transitions at the %dth place.\n", i);
                    return 1;
                }
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 40){
            if(a == ')'){
                etat = 45;
                fputc(a, output);
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 41){
            if(a == ','){
                etat++;
                fputc(a, output);
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 42){
            if(a == '`'){
                etat++;
                fputc(a, output);
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 43){
            if(is_number(a) || is_letter(a)){
                etat++;
                fputc(a, output);
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 44){
            if(a == '`'){
                etat = 40;
                fputc(a, output);
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 45){
            if(a == ','){
                etat = 34;
                fputc(a, output);
            }
            else if(a == ')'){
                etat++;
                fputc(a, output);
                fputc('"', output);
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 46){
            if(a == ',') etat++;
            else if(a == ']') etat = 48;
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 47){
            if(a == '(') etat = 24;
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
        else if(etat == 48){
            if(a == '}'){
                fputs(";\n", output);
                fclose(output);
                return 0;
            }
            else{
                fclose(output);
                printf("can't read the transitions at the %dth place.\n", i);
                return 1;
            }
        }
    }
    return 0;

}

int main(){
    CharList* res = read_file();
    printf("There are %d characters.\n", res->size);

    int error = analyse_syntaxique(res);

    if(error == 1) printf("Syntax Error.\n");
    else printf("No syntax error.\n");

    return 0;
}