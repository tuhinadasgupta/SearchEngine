#include "searchengine.h"
#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//Function to take set of ducments and create hashmap of given size with word/document id pairs
struct hashmap* training(int size, struct doc* doc_head){
  //Create hashmap of given size
  struct hashmap* hm = hm_create(size);

  //move through each document to populate hashmap
  FILE* doc;
  char* word;
  char line[256];
  while(doc_head !=  NULL){
    hm->num_docs++; //Increment total number of documents
    doc = fopen(doc_head->document_id, "r");
    if(doc != NULL){  //If document is found, continue
      while(fgets(line, 256, doc) != NULL){ //While EOF is not reached, get line from file
        word = strtok(line, " \n");   //Split line at space characters
        while(word != NULL){  //While there are still words in a line:
          hm_put(hm, strdup(word), doc_head->document_id);
          word = strtok(NULL, " \n"); //Get next word in line
        }
      }
    }
    fclose(doc);
    doc_head = doc_head->next;
  }

  //Remove all stop words form hashmap;
  stop_word(hm);
  return hm;
}

//Read search query from user and return linked list of words
struct query* read_query(void){
  //Create query (linked list) of size 0
  struct query* query = (struct query*)malloc(sizeof(struct query));
  query->size = 0;
  query->head = NULL;

  char* word;
  char line[256];
  //Prompt user for query
  printf("Enter a search query (press enter when done):\n");
  //Read in query word by word
  while(fgets(line, 256, stdin) != NULL){
    word = strtok(line, " \n");
    while(word != NULL){
      //Add each word to the query list
      addWord(query, word);
      word = strtok(NULL, " \n");
    }
    //If user hits enter, break the loop
    if(line[strlen(line)-1]=='\n'){
      break;
    }
  }
  return query;
}

void addWord(struct query* query, char* word){
  //If query is not initialized do not add words
  if(query == NULL){
    return;
  }
  //If query has no head, create first word node
  if(query->head == NULL){
    query->head = (struct word*)malloc(sizeof(struct word));
    query->head->str = strdup(word);
    query->head->next = NULL;
    query->size++;
    return;
  }
  //Otherwise iterate to end of query list
  struct word* ptr = query->head;
  while(ptr->next!=NULL){
    ptr = ptr->next;
  }
  //Add word to end of list
  ptr->next = (struct word*)malloc(sizeof(struct word));
  ptr->next->str = strdup(word);
  ptr->next->next = NULL;
  query->size++;
}

//Ranks all documents and prints them in a ranked list
void rank(struct hashmap* hm, struct doc* doc_head, struct query* query){
  //If query is not initialized, do not execute
  if(query == NULL){
    return;
  }
  int num_words = query->size;  //Total number of documents
  int i;
  int good_docs = 0;  //Counter to track documents that contain query
  struct doc* doc = doc_head;
  //While there are documents, get score for document
  while(doc != NULL){
    double score = 0;
    struct word* word = query->head;
    //Iterate through all words in query
    for(i=0; i<num_words; i++){
      //Get tf and df of word in document
      double tf = (double)hm_get_tf(hm, word->str, doc->document_id);
      double df = (double)hm_get_df(hm, word->str);
      //Get next word
      word = word->next;
      //If word/doc id pair are not in hashmap, do not add to score
      if(tf == -1 || df == -1){
        continue;
      }
      //Add tf-idf to toal document score
      score += (tf * log10(hm->num_docs/df));
    }
    //Set total score of document
    doc->score = score;
    //If score is greater than 0, increment number of useful documents
    if(score>0){
      good_docs++;
    }
    doc = doc->next;
  }

  //If no documnets contained the query, return
  if(good_docs==0){
    printf("There are no useful documents\n");
    return;
  }

  //Sort and print ranked documents
  sort(doc_head);

  //Free query
  struct word* curr = query->head;
  while(curr != NULL){
    struct word* temp = curr->next;
    free(curr->str);
    free(curr);
    curr = temp;
  }
  free(query);
}

void sort(struct doc* doc_head){
  struct doc* sorted = NULL;
  struct doc* curr = doc_head;
  //Iterate through document list and plac in sorted linked list
  while(curr!= NULL){
    //If document has score of 0, do not include in list
    if(curr->score == 0){
      curr = curr->next;
      continue;
    }
    //If sorted list head not initialized, make head
    if(sorted == NULL){
      sorted = malloc(sizeof(struct doc));
      sorted->document_id = curr->document_id;
      sorted->score = curr->score;
      sorted->next = NULL;
    }
    //If current doc has score greater than sorted head, make it new head
    else if(curr->score > sorted->score){
      struct doc* new_head = malloc(sizeof(struct doc));
      new_head->document_id = curr->document_id;
      new_head->score = curr->score;
      new_head->next = sorted;
      sorted = new_head;
    }else{
    //Otherwise place in appropriate place in list
      struct doc* next = sorted;
      while(next->next != NULL && curr->score<next->next->score){
        next = next->next;
      }
      struct doc* new = malloc(sizeof(struct doc));
      new->document_id = curr->document_id;
      new->score = curr->score;
      new->next = next->next;
      next->next = new;
    }
    curr = curr->next;
  }

  //Print sorted list
  curr = sorted;
  while(curr!= NULL){
    printf("%s\n", curr->document_id);
    curr = curr->next;
  }

  //Free sorted list
  curr = sorted;
  while(curr != NULL){
    struct doc* temp = curr->next;
    free(curr);
    curr = temp;
  }
}

//identifies stop words and removes them from the hashmap
void stop_word(struct hashmap* hm){
  //If no documents have been read (ie training not executed) return
  if(hm->num_docs==0){
    return;
  }
  int i;
  //Iterate through all buckets in hashmap
  for(i=0; i<hm->num_buckets; i++){
    struct word_node* ptr = (hm->map)[i];
    //Iterate through all words in bucket
    while(ptr != NULL){
      //If word is a stop word, remove it from the hashmap
      if(ptr->df == hm->num_docs){
        hm_remove_word(hm, ptr->word);
      }
      ptr = ptr->next_word;
    }
  }

}
