#ifndef HASHMAP_H
#define HASHMAP_H

struct llnode {
        char* word;
        char* document_id;
        int num_occurrences;
        struct llnode* next;
};

struct hashmap {
        struct llnode** map;
        int num_buckets;
        int num_elements;
};


struct hashmap* hm_create(int num_buckets);
int hm_get(struct hashmap* hm, char* word, char* document_id);
void hm_put(struct hashmap* hm, char* word, char* document_id, int num_occurrences);
void hm_remove(struct hashmap* hm, char* word, char* document_id);
void hm_destroy(struct hashmap* hm);
int hash(struct hashmap* hm, char* word);
struct hashmap* stopWords(struct hashmap* hm);
struct hashmap* training(int num_buckets);
void freeGlob(int docTotal, char** docList);
double calcIDF(struct hashmap* hm, char* word, int docTotal);
int calcDF(struct hashmap* hm, char* word);
int calcTF(struct hashmap* hm, char* word, char* document_id);
void searchQuery (struct hashmap* hm, int docTotal,char** docList, char* input);
double calcTF_IDF (struct hashmap* hm, char* word, char* document_id, int docTotal);
void sort(char** docList ,int start, int end, double *rankArr);
void swap(char** docList, double *rankArr, int h, int l);
void write(char** docList, int docTotal, double *rankArr);



#endif
