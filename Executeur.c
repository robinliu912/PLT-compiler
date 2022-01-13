#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

// store the information of virtual machine
typedef struct vm
{
    int vm[1000];
    int etat_name[100][10];
    int etat_adr[100];
    int num_etat;
    int elem[2];
} vm;

typedef struct CharList_pointer
{
	int size;
    int pointer;
	char* content;
} CharList_p;

CharList_p* read_file(char* filename){
    FILE* fp = NULL;
    fp = fopen(filename, "r");

    if(fp == NULL){
        printf("Can't open the file\n");
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
    fclose(fp);

    return res;
}

// Initialize the vm
vm* init_vm(vm* vm){
    vm->num_etat = 0;
    for(int i=0; i<2; i++) vm->elem[i] = 0;
    for(int i=0; i<1000; i++) vm->vm[i] = -2;
    for(int i=0; i<100; i++) vm->etat_adr[i] = 0;
    for(int i=0; i<100; i++){
        for(int j=0; j<10; j++) vm->etat_name[i][j] = 0;
    }

    return vm;
}

vm* get_etat_name(vm* vm, CharList_p* l){
    char a;
    int n1 = 0, n2 = 0;
    for(int i=l->pointer; i<l->size; i++){
        a = l->content[i];
        if(a == '"'){
            a = l->content[++i];
            while(a != '"'){
                vm->etat_name[n1][n2++] = a;
                a = l->content[++i];
            }
            n1++;
            n2 = 0;
        }
    }
    vm->num_etat = n1;

    return vm;
}

int is_number(char a){
    if(a>='0' && a<='9') return 1;
    else return 0;
}

vm* get_etat_addresse(vm* vm, CharList_p* l){
    char a;
    int res = 0, n1 = 0, flag = 0;
    for(int i=l->pointer; i<l->size; i++){
        a = l->content[i];
        if(a == '='){
            flag = 0;
            i += 2;
            a = l->content[i];
            while(is_number(a)){
                flag = 1;
                res = res*10 + a - '0';
                a = l->content[++i];
            }
            if(flag == 1){
                vm->etat_adr[n1++] = res;
                res = 0;
            }  
        }
    }
    return vm;
}

vm* get_vm(vm* vm, CharList_p* l){
    char a;
    int res = 0, n1 = 0;
    for(int i=l->pointer; i<l->size; i++){
        a = l->content[i];
        if(a != ' ') res = res*10 + a - '0';
        else{
            if(res == -29) res = -1;
            vm->vm[n1++] = res;
            res = 0;
        }
    }

    return vm;
}

vm* get_element(vm* vm){
    if(vm->vm[0] == 0) return vm;
    else if(vm->vm[0] == 1){
        for(int i=0; i<vm->num_etat; i++){
            int adr = vm->etat_adr[i];
            for(int j=0; j<vm->vm[adr]; j++){  // not bigger than the export number
                if(vm->vm[adr+3+4*j] != 0){    // the character is at the third place of quadruplet
                    vm->elem[0] = vm->vm[adr+3+4*j];
                    return vm;
                }
            }
        }
    }
    else if(vm->vm[0] == 2){
        for(int i=0; i<vm->num_etat; i++){
            int adr = vm->etat_adr[i];
            for(int j=0; j<vm->vm[adr]; j++){
                // the first character is at the third place of 6-uplet
                if(vm->vm[adr+3+6*j] != 0) vm->elem[0] = vm->vm[adr+3+6*j];
                // the second character is at the 5th place of 6-uplet
                if(vm->vm[adr+5+6*j] != 0) vm->elem[1] = vm->vm[adr+5+6*j];
                if(vm->elem[0]!=0 && vm->elem[1]!=0) return vm;
            }
        }
    }
    return vm;
}

vm* get_all(CharList_p* l_vm, CharList_p* l_ts, vm* vm){
    vm = init_vm(vm);
    vm = get_etat_name(vm, l_ts);
    vm = get_etat_addresse(vm, l_ts);
    vm = get_vm(vm, l_vm);
    vm = get_element(vm);

    return vm;
}

void debug(vm* vm){
    printf("Donner le mot d'entree: ");
    char* str = (char*)malloc(200);
    scanf("%s", str);
    printf("  ->Etat: ");
    for(int i=0; i<vm->num_etat; i++){
        if(vm->etat_adr[i] == vm->vm[1]){
            int j = 0;
            while(vm->etat_name[i][j] != 0){
                printf("%c", vm->etat_name[i][j]);
                j++;
            } 
        }
    }

    int num_export = 0, flag = 0, index, index2;
    int pile1 = 0, pile2 = 0;
    if(vm->vm[0] == 0){
        printf("\n");
        index = vm->vm[1];
        for(int i=0; i<100; i++){
            if(str[i] != 0){
                printf("%c -> ", str[i]);
                flag = 0;
                num_export = vm->vm[index];
                for(int j=0; j<num_export; j++){
                    index2 = index+1+2*j;
                    if(str[i] == vm->vm[index2]){
                        flag = 1;
                        index = vm->vm[index2+1];
                        printf("Etat: ");
                        // find the name of etat
                        for(int t=0; t<vm->num_etat; t++){
                            if(vm->etat_adr[t] == index){
                                int m = 0;
                                while(vm->etat_name[t][m] != 0){
                                    printf("%c", vm->etat_name[t][m]);
                                    m++;
                                }
                            }
                        }
                    }
                }

                if(flag == 0){
                    printf("Erreur: On ne peut pas trouver la transition correspondante.\n");
                    printf("Le mot %s est refuse !\n", str);
                    return;
                }
                printf("\n");
            }
            else break;
        }

        // judge whether the current etat is final etat
        int num_final = vm->vm[2];
        for(int i=3; i<3+num_final; i++){
            if(vm->vm[i] == index){
                printf("Le mot %s est accepte !\n", str);
                return;
            }
        }

        for(int i=0; i<vm->num_etat; i++){
            if(vm->etat_adr[i] == index){
                printf("Le mot est refusee ! Etat ");
                int j = 0;
                while(vm->etat_name[i][j] != 0){
                    printf("%c", vm->etat_name[i][j]);
                    j++;
                }
                printf(" n'est pas un etat final. \n");
                return;
            }
        }
        return;
    }
    else if(vm->vm[0] == 1){
        printf(" Pile 1: Vide\n");
        index = vm->vm[1];
        for(int i=0; i<100; i++){
            if(str[i] != 0){
                printf("%c -> ", str[i]);
                flag = 0;
                num_export = vm->vm[index];
                for(int j=0; j<num_export; j++){
                    index2 = index+1+4*j;
                    if(str[i] == vm->vm[index2]){
                        flag = 1;
                        // check the pile
                        if(vm->vm[index2+3] == 1) pile1++;
                        else if(vm->vm[index2+3] == -1){
                            pile1--;
                            if(pile1<0){
                                printf("Erreur: Pile 1 vide !\n");
                                printf("Le mot %s est refusee !\n", str);
                                return;
                            }
                        }

                        index = vm->vm[index2+1];
                        printf("Etat : ");
                        // find the name of etat
                        for(int t=0; t<vm->num_etat; t++){
                            if(vm->etat_adr[t] == index){
                                int m = 0;
                                while(vm->etat_name[t][m] != 0){
                                    printf("%c", vm->etat_name[t][m]);
                                    m++;
                                }
                            }
                        }
                        printf(" Pile 1 : ");
                        for(int t=0; t<pile1; t++) printf("%c", vm->elem[0]);
                        if(pile1 == 0) printf("Vide");
                    }
                }
                // can't find the transition
                if(flag == 0){
                    printf("Erreur: On ne peut pas trouver la transition correspondante.\n");
                    printf("Le mot %s est refuse !\n", str);
                    return;
                }
                printf("\n");
            }
            else break;
        }
        // in case of pile non vide
        if(pile1 != 0){
            printf("Le mot %s est refuse ! Pile 1 non vide !\n", str);
            return;
        }

        // judge whether the current etat is final etat
        int num_final = vm->vm[2];
        for(int i=3; i<3+num_final; i++){
            if(vm->vm[i] == index){
                printf("Le mot %s est accepte !\n", str);
                return;
            }
        }

        for(int i=0; i<vm->num_etat; i++){
            if(vm->etat_adr[i] == index){
                printf("Le mot est refusee ! Etat ");
                int j = 0;
                while(vm->etat_name[i][j] != 0){
                    printf("%c", vm->etat_name[i][j]);
                    j++;
                }
                printf(" n'est pas un etat final. \n");
                return;
            }
        }
        return;
    }
    else if(vm->vm[0] == 2){
        printf(" Pile 1 : Vide Pile 2 : Vide\n");
        index = vm->vm[1];
        for(int i=0; i<100; i++){
            if(str[i] != 0){
                printf("%c -> ", str[i]);
                flag = 0;
                num_export = vm->vm[index];
                for(int j=0; j<num_export; j++){
                    index2 = index+1+6*j;
                    if(vm->vm[index2] == str[i]){
                        flag = 1;
                        // check the pile1
                        if(vm->vm[index2+3] == 1) pile1++;
                        else if(vm->vm[index2+3] == -1){
                            pile1--;
                            if(pile1<0){
                                printf("Erreur: Pile 1 vide !\n");
                                printf("Le mot %s est refusee !\n", str);
                                return;
                            }
                        }

                        // check the pile2
                        if(vm->vm[index2+5] == 1) pile2++;
                        else if(vm->vm[index2+5] == -1){
                            pile2--;
                            if(pile2<0){
                                printf("Erreur: Pile 1 vide !\n");
                                printf("Le mot %s est refusee !\n", str);
                                return;
                            }
                        }

                        index = vm->vm[index2+1];
                        printf("Etat: ");
                        // find the name of etat
                        for(int t=0; t<vm->num_etat; t++){
                            if(vm->etat_adr[t] == index){
                                int m = 0;
                                while(vm->etat_name[t][m] != 0){
                                    printf("%c", vm->etat_name[t][m]);
                                    m++;
                                }
                            }
                        }
                        printf(" Pile 1 : ");
                        for(int t=0; t<pile1; t++) printf("%c", vm->elem[0]);
                        if(pile1 == 0) printf("Vide");
                        printf(" Pile 2 : ");
                        for(int t=0; t<pile2; t++) printf("%c", vm->elem[1]);
                        if(pile2 == 0) printf("Vide");
                    }
                }
                if(flag == 0){
                    printf("Erreur: On ne peut pas trouver la transition correspondante.\n");
                    printf("Le mot %s est refuse !\n", str);
                    return;
                }
                printf("\n");
            }
            else break;
        }

        if(pile1!=0 && pile2==0){
            printf("Le mot %s est refusee ! Erreur : Pile 1 non vide !\n", str);
            return;
        }
        else if(pile1==0 && pile2!=0){
            printf("Le mot %s est refusee ! Erreur : Pile 2 non vide !\n", str);
            return;
        }
        else if(pile1!=0 && pile2!=0){
            printf("Le mot %s est refusee ! Erreur : Pile 1 et Pile 2 non vides !\n", str);
            return;
        }

        // judge whether the current etat is final etat
        int num_final = vm->vm[2];
        for(int i=3; i<3+num_final; i++){
            if(vm->vm[i] == index){
                printf("Le mot %s est accepte !\n", str);
                return;
            }
        }

        for(int i=0; i<vm->num_etat; i++){
            if(vm->etat_adr[i] == index){
                printf("Le mot est refusee ! Etat ");
                int j = 0;
                while(vm->etat_name[i][j] != 0){
                    printf("%c", vm->etat_name[i][j]);
                    j++;
                }
                printf(" n'est pas un etat final. \n");
                return;
            }
        }
        return;
    }
}

void exec(vm* vm){
    printf("Donner le mot d'entree: ");
    char* str = (char*)malloc(200);
    scanf("%s", str);
    int num_export = 0, flag = 0, index, index2;
    int pile1 = 0, pile2 = 0;
    if(vm->vm[0] == 0){
        index = vm->vm[1];
        for(int i=0; i<100; i++){
            if(str[i] != 0){
                flag = 0;
                num_export = vm->vm[index];
                for(int j=0; j<num_export; j++){
                    index2 = index+1+2*j;
                    if(str[i] == vm->vm[index2]){
                        flag = 1;
                        index = vm->vm[index2+1];
                    }
                }

                if(flag == 0){
                    printf("Le mot %s est refuse !\n", str);
                    return;
                }
            }
            else break;
        }

        // judge whether the current etat is final etat
        int num_final = vm->vm[2];
        for(int i=3; i<3+num_final; i++){
            if(vm->vm[i] == index){
                printf("Le mot %s est accepte !\n", str);
                return;
            }
        }
        printf("Le mot %s est refuse !\n", str);
        return;
    }
    else if(vm->vm[0] == 1){
        index = vm->vm[1];
        for(int i=0; i<100; i++){
            if(str[i] != 0){
                flag = 0;
                num_export = vm->vm[index];
                for(int j=0; j<num_export; j++){
                    index2 = index+1+4*j;
                    if(str[i] == vm->vm[index2]){
                        flag = 1;
                        // check the pile
                        if(vm->vm[index2+3] == 1) pile1++;
                        else if(vm->vm[index2+3] == -1){
                            pile1--;
                            if(pile1<0){
                                printf("Le mot %s est refusee !\n", str);
                                return;
                            }
                        }

                        index = vm->vm[index2+1];
                    }
                }
                // can't find the transition
                if(flag == 0){
                    printf("Le mot %s est refuse !\n", str);
                    return;
                }
            }
            else break;
        }
        // in case of pile non vide
        if(pile1 != 0){
            printf("Le mot %s est refuse ! Pile 1 non vide !\n", str);
            return;
        }

        // judge whether the current etat is final etat
        int num_final = vm->vm[2];
        for(int i=3; i<3+num_final; i++){
            if(vm->vm[i] == index){
                printf("Le mot %s est accepte !\n", str);
                return;
            }
        }
        printf("Le mot %s est refuse !\n", str);
        return;
    }
    else if(vm->vm[0] == 2){
        index = vm->vm[1];
        for(int i=0; i<100; i++){
            if(str[i] != 0){
                flag = 0;
                num_export = vm->vm[index];
                for(int j=0; j<num_export; j++){
                    index2 = index+1+6*j;
                    if(vm->vm[index2] == str[i]){
                        flag = 1;
                        // check the pile1
                        if(vm->vm[index2+3] == 1) pile1++;
                        else if(vm->vm[index2+3] == -1){
                            pile1--;
                            if(pile1<0){
                                printf("Le mot %s est refusee !\n", str);
                                return;
                            }
                        }

                        // check the pile2
                        if(vm->vm[index2+5] == 1) pile2++;
                        else if(vm->vm[index2+5] == -1){
                            pile2--;
                            if(pile2<0){
                                printf("Le mot %s est refusee !\n", str);
                                return;
                            }
                        }

                        index = vm->vm[index2+1];
                    }
                }
                if(flag == 0){
                    printf("Le mot %s est refuse !\n", str);
                    return;
                }
            }
            else break;
        }

        if(pile1!=0 && pile2==0){
            printf("Le mot %s est refusee ! Erreur : Pile 1 non vide !\n", str);
            return;
        }
        else if(pile1==0 && pile2!=0){
            printf("Le mot %s est refusee ! Erreur : Pile 2 non vide !\n", str);
            return;
        }
        else if(pile1!=0 && pile2!=0){
            printf("Le mot %s est refusee ! Erreur : Pile 1 et Pile 2 non vides !\n", str);
            return;
        }

        // judge whether the current etat is final etat
        int num_final = vm->vm[2];
        for(int i=3; i<3+num_final; i++){
            if(vm->vm[i] == index){
                printf("Le mot %s est accepte !\n", str);
                return;
            }
        }
        printf("Le mot %s est refuse !\n", str);
        return;
    }
}

int main(int argc, char* argv[]){
    if(argc!=2 && argc!=3){
        printf("Please input the correct command!\n");
        return 1;
    }
    char* filename = argv[argc-1];
    strcat(filename, ".txt");
    CharList_p* l_ts = read_file("TS.txt");
    CharList_p* l_vm = read_file(filename);
    vm vm1;
    vm* vm = &vm1;
    vm = get_all(l_vm, l_ts, vm);
    
    if(strcmp(argv[1], "-debug")==0 && argc==3) debug(vm);
    else if(argc==2) exec(vm);
    else{
        printf("Please input the correct command!\n");
        return 1;
    }

    return 0;
}