#include "hashmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <glob.h>
#include <stdbool.h>

#define MAX_LEN 1024

int main(void){
        int num_buckets;
        printf("How many buckets?:\n");
  
        scanf("%d",&num_buckets);    

        //enter the string or x
        bool escape = true;
        while(escape){
                struct hashmap* hm = training(num_buckets);

                int docTotal;
                char** docList;
                glob_t globStruct;
                int globVal = glob("./p5docs/*.txt", 0, NULL, &globStruct);
                if(globVal != 0) {
                        return 0;
                }
                //num of docs
                docTotal = globStruct.gl_pathc;
                //array of doc paths
                docList = globStruct.gl_pathv;

                char input[MAX_LEN];
                printf("Enter search string or X to exit:\n");

                //fix for something idk
                scanf("%*[^\n]"); 
                scanf("%*c");

                scanf("%[^\n]", input);
                //print test for input
                //printf("%s\n", input);
                
                if((strcmp(input, "X") == 0) || (strcmp(input,"x")==0)) {
                        hm_destroy(hm);
                        escape = false;
                        freeGlob(docTotal, docList);
                        exit(0);
                } else {
                        searchQuery(hm, docTotal, docList, input);
                        hm_destroy(hm);
                        //freeGlob(docTotal, docList);
                }
        }
}