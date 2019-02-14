#ifndef HASHMAP_H
#define HASHMAP_H

struct doc_node {
        char* document_id;
        int num_occurrences;
        struct doc_node* next;
};

struct word_node {
  char* word;
  int df;
  struct word_node* next_word;
  struct doc_node* next_doc;
};

struct hashmap {
        struct word_node** map;
        int num_buckets;
        int num_docs;
};

struct hashmap* hm_create(int num_buckets);
void hm_put(struct hashmap* hm, char* word, char* document_id);
int hm_get_tf(struct hashmap* hm, char* word, char*document_id);
int hm_get_df(struct hashmap* hm, char* word);
void hm_remove_word(struct hashmap* hm, char* word);
void hm_destroy(struct hashmap* hm);
int hash(struct hashmap* hm, char* word);
void hm_print(struct hashmap* hm);

#endif
