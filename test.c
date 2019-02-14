#include <stdio.h>
#include <stdlib.h>
#include "hashmap.h"
#include "searchengine.h"
#include <string.h>

int main(void){
  //Get documents names in linked list
  struct doc* docs = malloc(sizeof(struct doc));
  struct doc* doc2 = malloc(sizeof(struct doc));
  struct doc* doc3 = malloc(sizeof(struct doc));
  docs->document_id = "D1.txt";
  docs->next = doc2;
  doc2->document_id = "D2.txt";
  doc2->next = doc3;
  doc3->document_id = "D3.txt";
  doc3->next = NULL;

  //Read hashmap size from user
  int size;
  char line[4];
  printf("Enter a number of buckets for the hashmap (less than 1000): ");
  fgets(line, 4, stdin);
  printf("\n");
  size = atoi(line);

  //Execute training phase to make populated hashmap and remove all stop stopwords
  struct hashmap* hm = training(size, docs);

  struct query* query;
  printf("Enter # to exit the program\n");
  while(1){
    //Get search query from user
    query = read_query();
    //If user enters #, free query andexit program
    if(strcmp(query->head->str, "#") == 0){
      struct word* curr = query->head;
      while(curr != NULL){
        struct word* temp = curr->next;
        free(curr->str);
        free(curr);
        curr = temp;
      }
      free(query);
      break;
    }
    //Print ranked documents to user
    printf("These are the documents containing your query ranked in order of importance:\n");
    rank(hm, docs, query);
  }

  //Free hashamp and documents when done
  hm_destroy(hm);
  struct doc* doc = docs;
  while(doc != NULL){
    struct doc* temp = doc->next;
    free(doc);
    doc = temp;
  }

}
