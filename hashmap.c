#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Create a hashmap with specified number of buckets
struct hashmap* hm_create(int num_buckets){
  struct hashmap* hm;
  hm = malloc(sizeof(struct hashmap));  //Allocate memory for Hashmap
  hm->map = (struct word_node**)malloc(sizeof(struct word_node*)*num_buckets);  //Allocate space for each bucket in map
  hm->num_buckets = num_buckets;
  hm->num_docs = 0;
  int i;
  for(i=0; i<num_buckets; i++){
    (hm->map)[i] = NULL;  //Initilize each bucket to null
  }
  return hm;
}

//Put a key value pair into the hashmap
//If pair already exists, overwrite num_occurence value
void hm_put(struct hashmap* hm, char* word, char* document_id){
  //If list has not been initialized, return
  if(hm == NULL){
    printf("Hashmap not initialized\n");
    return;
  }
  int key = hash(hm, word);  //Get key for data pair
  if((hm->map)[key] == NULL){ //If list has not been initilized, create first node
    (hm->map)[key] = (struct word_node*)malloc(sizeof(struct word_node));
    (hm->map)[key]->word = word;
    (hm->map)[key]->df = 1; //New word, so doc freq = 1
    (hm->map)[key]->next_word = NULL;
    (hm->map)[key]->next_doc = (struct doc_node*)malloc(sizeof(struct doc_node));
    (hm->map)[key]->next_doc->document_id = document_id;
    (hm->map)[key]->next_doc->num_occurrences = 1;
    (hm->map)[key]->next_doc->next = NULL;
    return;
  }

  //If word matches list head
  if(strcmp((hm->map)[key]->word, word) == 0){
    struct doc_node* ptr = (hm->map)[key]->next_doc;
    //If document id matches first document of word
    if(strcmp(ptr->document_id, document_id) == 0){
      ptr->num_occurrences+=1;
      return;
    }
    //Iterate through documents to find matching document
    while(ptr->next != NULL){
      ptr = ptr->next;
      if(strcmp(ptr->document_id, document_id) == 0){
        //printf("%s\n", ptr->document_id);
        //printf("%d\n",ptr->num_occurrences);
        ptr->num_occurrences+=1;
        //printf("%d\n",ptr->num_occurrences);
        return;
      }
    }
    //If no document matches add a document node
    (hm->map)[key]->df+=1; //Increment doc requency becasue of new document nodes
    ptr->next = malloc(sizeof(struct doc_node));
    ptr->next->document_id = document_id;
    ptr->next->num_occurrences = 1;
    ptr->next->next = NULL;
    return;
  }
  struct word_node* ptr2 = (hm->map)[key];
  //Iterate through list to find data pair
  while(ptr2->next_word != NULL){
    ptr2 = ptr2->next_word;
    //If data matches current node, set num_occurences return
    if(strcmp(ptr2->word, word) == 0){
      struct doc_node* ptr = ptr2->next_doc;
      //If document id matches first document of word
      if(strcmp(ptr->document_id, document_id) == 0){
        ptr->num_occurrences+=1;
        return;
      }
      //Iterate through documents to find matching document
      while(ptr->next != NULL){
        ptr = ptr->next;
        if(strcmp(ptr->document_id, document_id) == 0){
          ptr->num_occurrences+=1;
          return;
        }
      }
      //If no document matches add a document node
      ptr2->df+=1; //Increment doc frequency
      ptr->next = malloc(sizeof(struct doc_node));
      ptr->next->document_id = document_id;
      ptr->next->num_occurrences = 1;
      ptr->next->next = NULL;
      return;
    }
  }
  //If data pair not in list, make new node at end of list
  ptr2->next_word = malloc(sizeof(struct word_node));
  ptr2->next_word->word = word;
  ptr2->next_word->df = 1;
  ptr2->next_word->next_word = NULL;
  ptr2->next_word->next_doc = malloc(sizeof(struct doc_node));
  ptr2->next_word->next_doc->document_id = document_id;
  ptr2->next_word->next_doc->num_occurrences = 1;
  ptr2->next_word->next_doc->next = NULL;

}

//Get term frequency of word in a document by finding word/doc id pair
int hm_get_tf(struct hashmap* hm, char* word, char*document_id){
  //If list has not been initialized, return -1
  if(hm == NULL){
    printf("Hashmap not initialized\n");
    return -1;
  }
  int key = hash(hm, word);
  if((hm->map)[key] == NULL){   //If list head is null, return -1
    return -1;
  }
  struct doc_node* docptr = (hm->map)[key]->next_doc;
  //If word is found look for document id
  if(strcmp((hm->map)[key]->word, word)==0){
    while(docptr != NULL){
      if(strcmp(docptr->document_id, document_id) == 0){
        return docptr->num_occurrences;
      }
      docptr = docptr->next;
    }
    return -1;
  }
  //Iterate through list to find word df
  struct word_node* ptr = (hm->map)[key];
  while(ptr->next_word != NULL){
    ptr = ptr->next_word;
    if(strcmp(ptr->word, word) == 0){
      docptr = ptr->next_doc;
      while(docptr != NULL){
        if(strcmp(docptr->document_id, document_id) == 0){
          return docptr->num_occurrences;
        }
        docptr = docptr->next;
      }
      return -1;
    }
  }
  return -1;  //if not in list, return -1

}

//Get document frequency of word node in map
int hm_get_df(struct hashmap* hm, char* word){
  //If list has not been initialized, return -1
  if(hm == NULL){
    printf("Hashmap not initialized\n");
    return -1;
  }
  int key = hash(hm, word);
  if((hm->map)[key] == NULL){   //If list head is null, return -1
    return -1;
  }
  if(strcmp((hm->map)[key]->word, word)==0){
    return (hm->map)[key]->df;
  }
  //Iterate through list to find word df
  struct word_node* ptr = (hm->map)[key];
  while(ptr->next_word != NULL){
    ptr = ptr->next_word;
    if(strcmp(ptr->word, word) == 0){
      return ptr->df;
    }
  }
  return -1;  //if not in list, return -1
}

//Remove a word node and all its document ndoes, used by stop word in search engine
void hm_remove_word(struct hashmap* hm, char* word){
  //If hashmap has not been initilizaed, return
  if(hm == NULL){
    printf("Hashmap not initialized\n");
    return;
  }
  int key = hash(hm, word);
  //If list has not been initlized, data pair not in list so return
  if((hm->map)[key] == NULL){
    return;
  }
  struct word_node* ptr = (hm->map)[key];
  struct word_node* ptr2;
  struct doc_node* docptr;
  if(strcmp(ptr->word, word)==0){
    //set first node to next_doc
    (hm->map)[key] = ptr->next_word;
    //remove word and all of its documents
    docptr = ptr->next_doc;
    while(docptr != NULL){
      struct doc_node* temp = docptr->next;
      free(docptr);
      docptr = temp;
    }
    free(ptr->word);
    free(ptr);
    return;
  }
  while(ptr->next_word != NULL){
    //if word node is found, remove it and all its documents
    if(strcmp(ptr->next_word->word, word)==0){
      //set next word
      ptr2 = ptr->next_word;
      ptr->next_word = ptr->next_word->next_word;
      //remove all documents
      docptr = ptr2->next_doc;
      while(docptr != NULL){
        struct doc_node* temp = docptr->next;
        free(docptr);
        docptr = temp;
      }
      free(ptr2->word);
      free(ptr2);
      return;
    }
    ptr = ptr->next_word;
  }
}

//Free all memory of hashmap
void hm_destroy(struct hashmap* hm){
  if(hm == NULL){
    printf("Hashmap not initialized\n");
    return;
  }
  int i;
  //Free every list in map
  for(i=0; i<hm->num_buckets; i++){
    struct word_node* ptr = (hm->map)[i];
    while(ptr != NULL){
      struct word_node* temp = ptr->next_word;
      hm_remove_word(hm, ptr->word);
      ptr = temp;
    }
  }
  //Free map
  free(hm->map);
  //free hashmap
  free(hm);
}


//take given word and maps them to correct bucket in hashmap
//return key
int hash(struct hashmap* hm, char* word){
  int sum = 0;
  int key;
  char* ptr = word;
  //Add ASCII values of word to sum
  while(*ptr != '\0'){
    sum += *ptr++;
  }
  //Key is sum mod 4
  key = sum%(hm->num_buckets);
  return key;
}

void hm_print(struct hashmap* hm){
  if(hm == NULL){
    printf("Hashmap not initialized\n");
    return;
  }
  int i;
  struct word_node* ptr;
  struct doc_node* doc;
  for(i=0; i<hm->num_buckets; i++){
    printf("----------Key = %d------------\n", i);
    ptr = (hm->map)[i];
    if(ptr == NULL){
      continue;
    }
    printf("---Word: %s DF: %d---\n", ptr->word, ptr->df);
    doc = ptr->next_doc;
    printf("Doc: %s Count: %d\n", doc->document_id, doc->num_occurrences);
    while(doc->next != NULL){
      doc = doc->next;
      printf("Doc: %s Count: %d\n", doc->document_id, doc->num_occurrences);
    }
    while(ptr->next_word != NULL){
      ptr = ptr->next_word;
      printf("---Word: %s DF: %d---\n", ptr->word, ptr->df);
      doc = ptr->next_doc;
      printf("Doc: %s Count: %d\n", doc->document_id, doc->num_occurrences);
      while(doc->next != NULL){
        doc = doc->next;
        printf("Doc: %s Count: %d\n", doc->document_id, doc->num_occurrences);
      }
    }
  }
}
