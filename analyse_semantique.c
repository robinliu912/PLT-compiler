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

CharList_p* read_file(){
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

int is_number(char a){
    if(a>='0' && a<='9') return 1;
    else return 0;
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

// locate the pointer to the front of next line
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


int main(){
    CharList_p* res = read_file();

    int error0 = is_transitions_null(res);
    int error1 = is_num_pile_correct(res);
    int error2 = is_num_etat_correct(res);
    int error3 = is_pile_elem_correct(res);

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
    return 0;
}