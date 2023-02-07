#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"
#include <glob.h>
#include <math.h>

#define maxWords 1024

struct hashmap* hm_create(int num_buckets){
    struct hashmap* hm;
    hm = malloc(sizeof(struct hashmap));//allocating memory for hashmap
    hm -> num_buckets = num_buckets; //sets the num_buckets to the passed value
    hm -> map = malloc (sizeof(struct llnode*) * num_buckets);
    hm->num_elements = 0;
    for(int i = 0; i < num_buckets; i ++){
        hm -> map[i] = NULL;
    }
    return hm;
}

int hm_get(struct hashmap* hm, char* word, char* document_id){
    //gets the key and set current to the current bucket
    int hashKey = hash(hm, word);
    struct llnode* current = hm->map[hashKey];

    //the bucket exists then return the num of occurrences
    while(current != NULL) { 
        //doc and word checks
        if(strcmp(current->document_id,document_id) == 0 && strcmp(current->word,word) == 0) {
            return current->num_occurrences;
        }
        current = current->next;
    }
    return -1;
}

void hm_put(struct hashmap* hm, char* word, char* document_id, int num_occurrences){
    //null case
    if(hm == NULL){
        printf("Hashmap NULL\n");
        return;
    }
    //gets the hashkey for the bucket with the word. and current has current node
    int hashKey = hash(hm, word);
    struct llnode* current = hm->map[hashKey];

    //if first bucket is empty we have to create a new node.
    if(current == NULL) {
        //allocating memory for the head of the new node
        struct llnode* new = malloc(sizeof(struct llnode));
        //allocating memory for the word array and  + 1 for null;
        new->word = malloc(sizeof(char)*strlen(word)+1);
        strcpy(new->word,word);
        //allocating mem for document_id and again +1 for null;
        new->document_id = malloc(sizeof(char)*strlen(document_id)+1);
        strcpy(new->document_id, document_id);

        new->num_occurrences = num_occurrences;
        new->next = NULL;
        
        //cant use current here bc of mem leak will happen
        hm->map[hashKey] = new;
        //do we ever use num elements???
        hm->num_elements++;
        return;
    } else if(strcmp(current->document_id,document_id) == 0 && strcmp(current->word,word) == 0){
        current->num_occurrences++;
        return;
    }
    //iterate through the linked list by looking one ahead and checking the current node
    while(current){
        if(strcmp(current->document_id,document_id) == 0 && strcmp(current->word,word) == 0){
            current->num_occurrences++;
            return;
        }
        if(current->next == NULL){
            break;
        }
        current = current->next;
    }
    //add new node if the word doesnt already exist.
    struct llnode* new2 = malloc(sizeof(struct llnode));
    new2->word = malloc(sizeof(char)*strlen(word)+1);
    strcpy(new2->word, word);
    //allocating memory for the word array and  + 1 for null;
    new2->document_id = malloc(sizeof(char)*strlen(document_id)+1);
    strcpy(new2->document_id, document_id);
    //allocating mem for document_id
    new2->num_occurrences = num_occurrences;
    new2->next = NULL;

    current->next = new2;
    hm->num_elements++;
    return;
}

void hm_remove(struct hashmap* hm, char* word, char* document_id) {
    //null case
    if(hm == NULL){
        return;
    }
    //getting hashey
    int hashKey = hash(hm, word);
    struct llnode *current = hm->map[hashKey];
    struct llnode *next = current;
    if(current == NULL) {
        return;
    }
    //if word then free and delete the node
    if(strcmp(current->word, word) == 0){
        next = current->next;
        free(current->word);
        free(current->document_id);
        free(current);
        hm->map[hashKey] = next;
    return;
  }
    while(current->next != NULL) {
        if(strcmp(current->document_id,document_id) == 0 && strcmp(current->word,word) == 0) {
            next->next = current ->next;
            free(current->word);
            free(current->document_id);
            free(current);
            hm->num_elements -= 1;
            return;
        }
        next = current;
        current = current->next;
    }
    //case for the last node.
    if(strcmp(current->word, word) == 0 && strcmp(current->document_id, document_id) == 0) {
        hm->num_elements = hm->num_elements - current->num_occurrences;
        next->next = current-> next;
        free(current->word);
        free(current->document_id);
        free(current);
        return;
    }
    return;
}

void hm_destroy(struct hashmap* hm){
    struct llnode *current;
    struct llnode *holder;
    for(int i = 0; i < hm->num_buckets; i ++) {
        current = hm->map[i];
        while(current != NULL) {
            holder = current;
            current = current->next;
            free(holder->document_id);
            free(holder->word);
            free(holder);
        }
    }
    free(hm->map);
    free(hm);
}

int hash(struct hashmap* hm, char* word){

    int hashKey;
    int sum_ascii = 0;
    int length = strlen(word);
    
    for(int i = 0; i < length; i++) {
        sum_ascii += word[i];
    }
    
    hashKey = (int)sum_ascii % (hm ->num_buckets);
    return hashKey;
}

struct hashmap* stopWords(struct hashmap* hm){
    //if hm is emtpy just return hm
    //printf("hehheeh");
    double IDF = 0.0; // Initializes idfVal 
    int docTotal;
    char** docList;
    glob_t glob_struct;
            int globVal = glob("./p5docs/*.txt", 0, NULL, &glob_struct);
            if(globVal != 0){
                    return 0;
            }
    docTotal = glob_struct.gl_pathc;
    docList = glob_struct.gl_pathv;


    if(hm == NULL) {
        printf("hashmap is emtpy");
        return hm;
    }
    struct llnode* current;
    
    int i; 
    //For loop iterates through buckets 
    for(i = 0; i < hm -> num_buckets; i++){
        current = hm -> map[i]; //Initializes iterator 
        if(current == NULL){ //If iterator is empty, return 0 
            continue;; 
        } else {
        //While loop iterates through linked list
            while(current != NULL){
                IDF = calcIDF(hm, current -> word, docTotal);
                if(IDF == 0) {
                    char* wordR = (char*)malloc(sizeof(char)*strlen(current->word) + 1);
                    strcpy(wordR, current->word);
                    for(i = 0; i < docTotal; i++) {
                        hm_remove(hm, wordR, docList[i]);
                    }
                    free(wordR);
                } else {
                    current = current->next;
                }
            }
        }
    }
    freeGlob(docTotal, docList);
    return 0;
}

struct hashmap* training(int num_buckets){
        struct hashmap * hm = hm_create(num_buckets); //creates the hashmap with the specified buckets
        //glob

        int docTotal;
        char** docList;
        glob_t glob_struct;
        int globVal = glob("./p5docs/*.txt", 0, NULL, &glob_struct);
        if(globVal != 0){
                return 0;
        }
        //total amount of doc
        docTotal = glob_struct.gl_pathc;
        //array of doc path
        docList = glob_struct.gl_pathv;
        char word[2024];
        for(int i; i < docTotal; i ++) {
                FILE *fp = fopen(docList[i], "r");
                if(fp == NULL) {
                        printf("File is empty!");
                        fclose(fp);
                        exit(1);
                }
                int num_occurences = 1;
                while(fscanf(fp,"%s", word) != EOF) {
                        hm_put(hm, word, docList[i], num_occurences);       
                } 
                fclose(fp);
        }        
        freeGlob(docTotal, docList);
        stopWords(hm);
        //printhashmap(hm);
        //free doc


        return hm;

/*------------------------------------------------------------------------*/
        /*
        FILE * D1 = fopen("./p5docs/D1.txt", "r");
        FILE * D2 = fopen("./p5docs/D2.txt", "r");
        FILE * D3 = fopen("./p5docs/D3.txt", "r");

        char str[100];
        int num_occurrences = 1;
        while(fscanf(D1, "%1023s" , str) != EOF) {
                hm_put(hm, str, "D1", num_occurrences);
        }
        fclose(D1);
        
        while(fscanf(D2, "%1023s" , str) != EOF) {
                hm_put(hm, str, "D2", num_occurrences);
        }
        fclose(D2);

        while(fscanf(D3, "%1023s" , str) != EOF) {
                hm_put(hm, str, "D3", num_occurrences);
        }
        fclose(D3);

        stopWords(hm);
        */
}

void freeGlob(int docTotal, char** docList) {
        for(int i = 0; i < docTotal; i ++){
                free(docList[i]);
        }
        free(docList);
}

double calcIDF(struct hashmap* hm, char* word,int docTotal) {
    double IDF = 0;
    double DF = 0;

    DF = (double)calcDF(hm, word);
    /*in the case Df is 0 we need to add 1*/
    if(DF == 0) {
        DF =+ 1;
        IDF = log((double)docTotal/(double)(DF));
    } else {
        IDF = log((double)docTotal/(double)(DF));
    }

    return IDF;
}

int calcDF(struct hashmap* hm, char* word) {
    int DF = 0;
    if(hm == NULL){
    return -1;
    }
    if(word == NULL){
    return -1;
    }
    int hashKey = hash(hm,word);
    struct llnode* current = hm->map[hashKey];

    while(current != NULL) {
        if(strcmp(current->word, word) == 0){
            DF++;
        }
        current = current->next;
    }
    return DF;
}

int calcTF(struct hashmap* hm, char* word, char* document_id){
    int TF = 0;
    if(hm == NULL){
        printf("hashmap NULL.\n");
    return -1;
    }
    if(word == NULL){
        printf("word NULL.\n");
    return -1;
    }
    if(document_id == NULL){
        printf("document_id NULL.\n");
    return -1;
    }
    int hashKey = hash(hm,word);
    struct llnode* current = hm->map[hashKey];

    while(current != NULL) {
        if(strcmp(current->document_id,document_id) == 0 && strcmp(current->word,word) == 0) {
            TF = current->num_occurrences;
            return TF;
        }
        current=current->next;
    }
    return TF;
}

double calcTF_IDF (struct hashmap* hm, char* word, char* document_id, int docTotal) {
    double TF_IDF = 0;
    TF_IDF = calcTF(hm , word, document_id)*calcIDF(hm, word, docTotal);
    return TF_IDF;
}

void searchQuery (struct hashmap* hm, int docTotal, char** docList, char* input) {
    //getting words into array
    char *words[maxWords];
    char *curWord;
    curWord = strtok(input, " ");
    int i = 0;



    //store query words in words[i];
    while (curWord != NULL && i < maxWords) {
        words[i] = curWord;
        curWord = strtok(NULL, " ");
        i++;
    }
    //making rank scores
    double *rankArr = malloc(i * sizeof(double));

    //iterates throught the docs and gets the TF_IDF for each doc.
    for(int dT = 0; dT < docTotal; dT++) {
        rankArr[dT] = 0;
        for(int wT = 0; wT < i; wT++) {
            rankArr[dT] += calcTF_IDF(hm, words[wT], docList[dT], docTotal);
        }
    }
    //int n = sizeof(rankArr) / sizeof(rankArr[0]);
    sort(docList , 0, docTotal-1, rankArr);
    write(docList , docTotal, rankArr);
    free(rankArr);
}

void sort(char** docList ,int start, int end, double *rankArr) {
    if (start >= end) {
        return;
    }

    int pivot = rankArr[end];
    int left = start;
    int right = end - 1;

    while (left <= right) {
        if (rankArr[left] < pivot && rankArr[right] > pivot) {
            swap(docList, rankArr, left, right);
        }
        if (rankArr[left] >= pivot) {
            left++;
        }
        if (rankArr[right] <= pivot) {
            right--;
        }
    }
    swap(docList, rankArr, end, left);
    sort(docList ,start , left-1, rankArr);
    sort(docList ,left + 1, end, rankArr);
}

void swap(char** docList, double *rankArr, int h, int l) {
  // Declare a temporary variable to store the value of arr[i]
  double temp = rankArr[h];
  rankArr[h] = rankArr[l];
  rankArr[l] = temp;

  char *tempc = docList[h];
  docList[h] = docList[l];
  docList[l] = tempc;
}

void write(char** docList, int docTotal, double *rankArr) {
    FILE *fp;
    //fp = fopen("./p5search_scores.txt", "w+");
    fp = fopen("./p5search_scores.txt", "a");

    if(fp == NULL) {
        printf("error opening file.\n");
        exit(1);
    }
    for(int i = 0; i <docTotal; i++) {
        fprintf(fp,"%s:    %f\n",docList[i],rankArr[i]);
    }
    fprintf(fp,"\n");
    fclose(fp);
    
}

