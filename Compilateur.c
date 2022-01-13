#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

typedef struct CharList_pointer
{
	int size;
    int pointer;
	char* content;
} CharList_p;

CharList_p* read_file3(){
    FILE* fp = NULL;
    fp = fopen("resultat_syntaxique.txt", "r");

    if(fp == NULL){
        printf("Can't open the resultat_syntaxique.txt\n");
        return NULL;
    }
    
    char a;
    CharList_p* res = malloc(sizeof(CharList_p));
    res->pointer = 0;
	res->size = 0;
	res->content = malloc(sizeof(char)*1000);
    while((a = fgetc(fp))!=EOF){
        res->content[res->size] = a;
        res->size++;
    }

    return res;
}

typedef struct info{
    int num_pile;
    int num_etat;
    char etat_names[100][10];
    int initial;
    int num_final;
    int final[100];
    int transition[100][10];
    int elem[2];                // at most 2 piles
} info;

info* init_info(){
    info* transformation = malloc(sizeof(info));
    transformation->num_pile = 0;
    transformation->num_etat = 0;
    transformation->initial = 0;
    transformation->num_final = 0;
    for(int i=0; i<100; i++) transformation->final[i] = -1;

    for(int i=0; i<100; i++){
        for(int j=0; j<10; j++){
            transformation->transition[i][j] = -2;
            transformation->etat_names[i][j] = 0;
        }
    }

    for(int i=0; i<2; i++) transformation->elem[i] = 0;

    return transformation;
}

int is_number(char a){
    if(a>='0' && a<='9') return 1;
    else return 0;
}

int get_number2(CharList_p* l){
    int flag = 0, num = 0;
    char a;
    for(int i=l->pointer; i<l->size; i++){
        a = l->content[i];
        while(is_number(a)){
            flag = 1;
            num *= 10;
            num += a - '0';
            i++;
            a = l->content[i];
        }
        l->pointer = i;
        if(flag == 1) break;
    }

    if(flag == 0) return -1;  // not exist any number
    return num;
}

int get_character(CharList_p* l){
    char a, res;
    for(int i=l->pointer; i<l->size; i++){
        a = l->content[i];
        if(a == '`'){
            res = l->content[i+1];
            l->pointer = i+3;               // move the pointer
            break;
        }
    }

    return res;
}

void next_line(CharList_p* l){
    char a;
    for(int i=l->pointer; i<l->size; i++){
        a = l->content[i];
        if(a == '\n'){
            l->pointer = i+1;
            break;
        }
    }
    return;
}

info* get_pile_elem(info* info, CharList_p* l, int num_pile){
    char a;
    if(num_pile == 1){
        for(int i=l->pointer; i<l->size; i++){
            a = l->content[i];
            if(a == '"') break;
            else if(a == '`'){
                info->elem[0] = l->content[i+1];   // not move the pointer
                i += 3;
            }
        }
    }
    else if(num_pile == 2){
        char b;
        for(int i=l->pointer; i<l->size; i++){
            a = l->content[i];
            if(a == '"') break;
            else if(a == '`'){
                b = l->content[i+1];
                if(info->elem[0] == 0) info->elem[0] = b;
                else if(info->elem[0] != b) info->elem[1] = b;
                i += 3;
            }
        }
    }

    return info;
}

info* get_op_pile1(info* info, CharList_p* l, int n1, int n2){
    char a, b;
    for(int i=l->pointer; i<l->size; i++){
        a = l->content[i];
        if(a=='(' || a==')'){
            l->pointer = i;
            break;
        }
    }

    if(a == '('){
        b = l->content[l->pointer+1];
        if(b == '`'){
            info->transition[n1][n2] = -1;  // pull out
            l->pointer += 10;
        }
        else if(b < 0){
            info->transition[n1][n2] = 1; // push in
            l->pointer += 10;
        }
        else if(b == ')'){                // no operation on pile
            info->transition[n1][n2] = 0;
            l-> pointer += 3;
        }
    }
    else{
        info->transition[n1][n2] = 0;
        l->pointer += 1;
    }

    return info;
}

info* get_op_pile2(info* info, CharList_p* l, int n1, int n2){
    char a, b, c;
    for(int i=l->pointer; i<l->size; i++){
        a = l->content[i];
        if(a == ')'){
            if(info->transition[n1][n2] == -2) info->transition[n1][n2] = 0;
            if(info->transition[n1][n2+1] == -2) info->transition[n1][n2+1] = 0;
        }
        else if(a == '('){
            b = l->content[i+1];
            if(b == '`'){
                c = l->content[i+2];
                if(c == info->elem[0]) info->transition[n1][n2] = -1;
                else if(c == info->elem[1]) info->transition[n1][n2+1] = -1;
                i += 8;
            }
            else if(b < 0){
                c = l->content[i+6];
                if(c == info->elem[0]) info->transition[n1][n2] = 1;
                else if(c == info->elem[1]) info->transition[n1][n2+1] = 1;
                i += 8;
            }
        }
        else if(a == '"'){
            l->pointer = i+1;
            break;
        }
    }

    return info;
}

// get the names of etats
info* get_etat(info* info, CharList_p* l){
    while(l->content[l->pointer] != '"'){
        l->pointer++;
    }

    char a;
    int n1 = 0, n2 = 0, flag;
    for(int i=l->pointer; i<l->size; i++){
        flag = 0;
        a = l->content[i];
        if(a == ';'){
            l->pointer = i;
            break;
        }

        while(a != '"'){
            flag = 1;
            if(a < 0){
                char b = l->content[i+1];
                char c = l->content[i+2];
                info->etat_names[n1][n2++] = a;
                info->etat_names[n1][n2++] = b;
                info->etat_names[n1][n2++] = c;
                i += 2;
            }
            else{
                info->etat_names[n1][n2++] = a;
            }
            a = l->content[++i];
        }
        if(flag == 1){
            i--;
            n1++;
            n2 = 0; 
        }
        
    }

    info->num_etat = n1;
    return info;
}

info* get_transition(info* info, CharList_p* l){
    while(l->content[l->pointer] != '"'){
        l->pointer++;
    }

    int n1 = 0, n2 = 0;
    int in, out;
    for(int i=l->pointer; i<l->size; i++){
        in = get_number2(l);
        out = get_number2(l);
        if(in == -1) break;
        info->transition[n1][n2++] = in;
        info->transition[n1][n2++] = out;
        info->transition[n1][n2++] = get_character(l);

        if(info->num_pile == 1){
            if(info->elem[0] == 0) info = get_pile_elem(info, l, 1);
            info = get_op_pile1(info, l, n1, n2);
        }
        else if(info->num_pile == 2){
            if(info->elem[0]==0 || info->elem[1]==0) info = get_pile_elem(info, l, 2);
            info = get_op_pile2(info, l, n1, n2);
        }
        i = l->pointer;
        n1++;
        n2 = 0;
    }

    return info;
}

// get the information from the "resultat_syntaxique.txt"
info* get_info(CharList_p* l){
    info* info = init_info();
    l->pointer = 0;
    info->num_pile = get_number2(l);
    next_line(l);
    info = get_etat(info, l);
    next_line(l);
    info->initial = get_number2(l);
    next_line(l);
    // get final etats
    char a;
    int j = 0;
    for(int i=l->pointer; i<l->size; i++){
        a = l->content[i];
        if(a == ';') break;
        info->final[j++] = get_number2(l);
        i = l->pointer;
    }
    info->num_final = j;
    next_line(l);
    info = get_transition(info, l);

    return info;
}

typedef struct vm
{
    int vm[1000];
    int etat_name[100][10];
    int etat_adr[100];
} vm;

vm* init_vm(info* info){
    vm* virtual_machine = malloc(sizeof(vm));
    for(int i=0; i<1000; i++){
        virtual_machine->vm[i] = -2;
    }
    for(int i=0; i<100; i++) virtual_machine->etat_adr[i] = -1;
    for(int i=0; i<100; i++){
        for(int j=0; j<10; j++){
            virtual_machine->etat_name[i][j] = info->etat_names[i][j];
        }
    }

    return virtual_machine;
}

vm* get_vm_0(vm* vm, info* info, int index){
    for(int i=0; i<info->num_etat; i++){
        int num_export = 0;    // num of output number
        vm->etat_adr[i] = index;
        for(int j=0; j<100; j++){
            if(info->transition[j][0] == i) num_export++;
        }
        vm->vm[index++] = num_export;

        for(int j=0; j<100; j++){
            if(info->transition[j][0] == i){
                vm->vm[index++] = info->transition[j][2];  // character
                vm->vm[index++] = -3-info->transition[j][1]; // next node
            }
            else if(info->transition[j][0] == -2) break;
        }
    }

    index = 1;
    while(vm->vm[index] != -2){
        if(vm->vm[index] <= -3) vm->vm[index] = vm->etat_adr[-3-vm->vm[index]];
        index++;
    }

    return vm;
}

vm* get_vm_1(vm* vm, info* info, int index){
    for(int i=0; i<info->num_etat; i++){
        int num_export = 0;    // num of output number
        vm->etat_adr[i] = index;
        for(int j=0; j<100; j++){
            if(info->transition[j][0] == i) num_export++;
        }
        vm->vm[index++] = num_export;

        for(int j=0; j<100; j++){
            if(info->transition[j][0] == i){
                vm->vm[index++] = info->transition[j][2];  // character
                vm->vm[index++] = -3-info->transition[j][1]; // next node
                if(info->transition[j][3]==1 || info->transition[j][3]==-1){
                    vm->vm[index++] = info->elem[0];
                    vm->vm[index++] = info->transition[j][3];
                }
                else{
                    vm->vm[index++] = 0;
                    vm->vm[index++] = 0;
                }
            }
            else if(info->transition[j][0] == -2) break;
        }
    }

    index = 1;
    while(vm->vm[index] != -2){
        if(vm->vm[index] <= -3) vm->vm[index] = vm->etat_adr[-3-vm->vm[index]];
        index++;
    }

    return vm;
}

vm* get_vm_2(vm* vm, info* info, int index){
    for(int i=0; i<info->num_etat; i++){
        int num_export = 0;    // num of output number
        vm->etat_adr[i] = index;
        for(int j=0; j<100; j++){
            if(info->transition[j][0] == i) num_export++;
        }
        vm->vm[index++] = num_export;

        for(int j=0; j<100; j++){
            if(info->transition[j][0] == i){
                vm->vm[index++] = info->transition[j][2];  // character
                vm->vm[index++] = -3-info->transition[j][1]; // next node
                if(info->transition[j][3]==-1 || info->transition[j][3]==1){
                    vm->vm[index++] = info->elem[0];
                    vm->vm[index++] = info->transition[j][3];
                }
                else{
                    vm->vm[index++] = 0;
                    vm->vm[index++] = 0;
                }

                if(info->transition[j][4]==-1 || info->transition[j][4]==1){
                    vm->vm[index++] = info->elem[1];
                    vm->vm[index++] = info->transition[j][4];
                }
                else{
                    vm->vm[index++] = 0;
                    vm->vm[index++] = 0;
                }
            }
            else if(info->transition[j][0] == -2) break;
        }
    }

    index = 1;
    while(vm->vm[index] != -2){
        if(vm->vm[index] <= -3) vm->vm[index] = vm->etat_adr[-3-vm->vm[index]];
        index++;
    }

    return vm;
}

int generate_vm(info* info){
    FILE* ts, *vm_file;
    ts = fopen("TS.txt", "w");
    vm_file = fopen("VM.txt", "w");

    if(ts == NULL){
        printf("Can't create the file TS.txt.\n");
        return 1;
    }
    if(vm_file == NULL){
        printf("Can't create the file VM.txt.\n");
        return 1;
    }

    vm* vm = init_vm(info);
    vm->vm[0] = info->num_pile;
    vm->vm[1] = -3-info->initial;
    vm->vm[2] = info->num_final;
    for(int i=0; i<info->num_final; i++){
        vm->vm[i+3] = -3-info->final[i];
    }
    int index = info->num_final+3;

    if(info->num_pile == 0) vm = get_vm_0(vm, info, index);
    else if(info->num_pile == 1) vm = get_vm_1(vm, info, index);
    else if(info->num_pile == 2) vm = get_vm_2(vm, info, index);

    // write to the TS.txt
    for(int i=0; i<info->num_etat; i++){
        fputs("Nom = \"", ts);
        int j = 0;
        while(vm->etat_name[i][j] != 0){
            fputc(vm->etat_name[i][j], ts);
            j++;
        }
        fputc('"', ts);
        fputs(", adresse = ", ts);
        fprintf(ts, "%d", vm->etat_adr[i]);
        fputs(";\n", ts);
    }

    for(int i=0; i<1000; i++){
        if(vm->vm[i] == -2) break;
        fprintf(vm_file, "%d ", vm->vm[i]);
    }
    fclose(ts);
    fclose(vm_file);
    return 0;
}

// copier les codes de analyse lexicle, syntaxique et semantique
// lexical
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

int is_chinese1(CharList* l, int index){
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
			else if(is_chinese1(l, i)){
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
			if(is_chinese1(l, i)) i += 2;
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

// syntaxique
CharList* read_file(){
    FILE* fp = NULL;
    fp = fopen("resultat_lexical.txt", "r");

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

int etat;

int analyse_syntaxique(CharList* l){
    etat = 0;
    FILE* output;
    output = fopen("resultat_syntaxique.txt", "w");
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

// semantique
CharList_p* read_file2(){
    FILE* fp = NULL;
    fp = fopen("resultat_syntaxique.txt", "r");

    if(fp == NULL){
        printf("Can't open the resultat_syntaxique.txt\n");
        return NULL;
    }
    
    char a;
    CharList_p* res = malloc(sizeof(CharList_p));
    res->pointer = 0;
	res->size = 0;
	res->content = malloc(sizeof(char)*1000);
    while((a = fgetc(fp))!=EOF){
        res->content[res->size] = a;
        res->size++;
    }

    return res;
}

int get_number(CharList_p* l){
    int flag = 0, num = 0;
    char a;
    for(int i=l->pointer; i<l->size; i++){
        a = l->content[i];
        if(a == '`'){
            i += 2;
            a = l->content[i];
        }
        while(is_number(a)){
            flag = 1;
            num *= 10;
            num += a - '0';
            i++;
            a = l->content[i];
        }
        l->pointer = i;
        if(flag == 1) break;
    }

    if(flag == 0) return -1;  // not exist any number
    return num;
}

int number_of_etat(CharList_p* l){
    int num = 0;
    char a;
    for(int i=l->pointer; i<l->size; i++){
        a = l->content[i];
        if(a == '"') num++;
        if(a == ';'){
            l->pointer = i;
            break;
        }
    }

    return num/2;
}

int is_transitions_null(CharList_p* l){
    l->pointer = 0;
    next_line(l);
    next_line(l);
    next_line(l);
    next_line(l);
    int num = get_number(l);
    if(num == -1){
        printf("Have no transitions.\n");
        return 1;
    }

    return 0;
}

int is_num_pile_correct(CharList_p* l){
    l->pointer = 0;
    int num_pile = get_number(l);
    next_line(l);
    next_line(l);
    next_line(l);
    next_line(l);

    int num_temp_pile = -1, num_paren = 0;
    char a;
    for(int i=l->pointer; i<l->size; i++){
        a = l->content[i];
        if(a == '('){
            num_paren++;
            num_temp_pile++;
        }
        else if(a == ')'){
            num_paren--;
        }

        if(num_paren == 0) num_temp_pile = -1;
        if(num_temp_pile > num_pile){
            printf("The number of piles is out of range.\n");
            return 1;
        }
    }

    return 0;
}

int is_num_etat_correct(CharList_p* l){
    l->pointer = 0;
    int num_state = 0, initial = 0, final = 0;
    next_line(l);
    num_state = number_of_etat(l);
    next_line(l);
    initial = get_number(l);
    if(initial >= num_state){
        printf("Initial state is out of range.\n");
        return 1;
    }

    next_line(l);
    final = get_number(l);
    if(final >= num_state){
        printf("Final state is out of range.\n");
        return 1;
    }

    next_line(l);
    for(int i=l->pointer; i<l->size; ){
        int etat = get_number(l);
        if(etat >= num_state){
            printf("Transitions state is out of range.\n");
            return 1;
        }

        if(etat == -1) return 0;
    }

    return 0;
}

int is_pile_elem_correct(CharList_p* l){
    l->pointer = 0;
    int num_pile  = get_number(l);
    int num_paren = 0, num_elem = 0, count = 0, flag = 0; // count is the number of '`'
    char elem[100] = {0};
    next_line(l);
    next_line(l);
    next_line(l);
    next_line(l);

    char a;
    for(int i=l->pointer; i<l->size; i++){
        a = l->content[i];
        if(a == '(') num_paren++;
        else if(a == ')') num_paren--;

        if(num_paren>1 && a=='`'){
            count++;
            if(count%2 == 1){
                a = l->content[++i];
                flag = 0;
                for(int j=0; j<num_pile; j++){
                    if(a == elem[j]) flag = 1;
                }

                if(flag == 0){
                    if(num_elem >= num_pile){
                        printf("The number of elements is bigger than pile number.\n");
                        return 1;
                    }
                    else{
                        elem[num_elem++] = a;
                    }
                }
            }
        }
    }

    return 0;
}


int main(int argc, char* argv[]){
    char* FILE_NAME = argv[1];
    FILE *fp = NULL, *fp_out = NULL;

	fp = fopen(FILE_NAME, "r");
    fp_out = fopen("resultat_lexical.txt", "w");
	
	if(fp == NULL)
	{
		printf("CAN NOT LOAD TXT FILE\n");
		return 1;
	}

    if(fp_out == NULL){
        printf("Can't create the file.\n");
        return 1;
    }
	
	CharList* res = remove_comment(fp);
	
    char a;
	for(int i=0; i<res->size; i++){
        a = res->content[i];
        if(a != ' ' && a!='\n' && a!='\t') fputc(a, fp_out);
		printf("%c", res->content[i]);
	}
	
	fclose(fp);
    fclose(fp_out);

	int error_character = character_correct(res);
	if(error_character == 0) printf("Characters are correct.\n");
	else printf("Error in characters.\n");

	int error_word = word_correct(res);
	if(error_word) printf("Error in word.\n");
	else printf("Words are correct.\n");

    CharList* res1 = read_file();
    // printf("%d\n", res->size);

    int error = analyse_syntaxique(res1);

    if(error == 1) printf("Syntax Error.\n");
    else printf("No syntax error.\n");

    CharList_p* res2 = read_file2();

    int error0 = is_transitions_null(res2);
    int error1 = is_num_pile_correct(res2);
    int error2 = is_num_etat_correct(res2);
    int error3 = is_pile_elem_correct(res2);

    if(error0 == 1) return 1;
    if(error1 == 1){
        printf("The number of piles is incorrect.\n");
        return 1;
    } 
    if(error2 == 1){
        printf("The number of etats is incorrect.\n");
        return 1;
    } 
    if(error3 == 1) return 1;

    if(error0 == 0 && error1 == 0 && error2 == 0 && error3 == 0) printf("No semantic errors.\n");

    CharList_p* l = read_file3();

    info* info = get_info(l);
/*
    for(int i=0; i<10; i++){
        for(int j=0; j<5; j++){
            printf("%c", info->etat_names[i][j]);
        }
    }
    printf("\n");

    printf("%c %c\n", info->elem[0], info->elem[1]);
    for(int i=0; i<10; i++){
        for(int j=0; j<10; j++){
            printf("%d ", info->transition[i][j]);
        }
    }
*/
    generate_vm(info);

    return 0;
}