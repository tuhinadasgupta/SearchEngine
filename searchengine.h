#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

struct doc {
  char* document_id;
  struct doc* next;
  double score;
};

struct query {
  struct word* head;
  int size;
};

struct word {
  char* str;
  struct word* next;
};



struct hashmap* training(int size, struct doc* doc_head);
struct query* read_query(void);
void addWord(struct query* query, char* word);
void rank(struct hashmap* hm, struct doc* doc_head, struct query* query);
void sort(struct doc* doc_head);
void stop_word(struct hashmap* hm);

#endif
