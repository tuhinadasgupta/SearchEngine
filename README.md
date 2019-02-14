# SearchEngine
Includes: 
- A function training for the “training” process, i.e., pre-processing, that takes a set of
documents as input and returns the populated hash table as output. Figure out the specifications
for the function.
- A function read_query to read the search query.
- A function rank in the search/retrieval process that computes the score for each document and
ranks them based on the tf-idf ranking algorithm. Your system should also determine if there is
no document with a match – i.e., if none of the words in the search query appear in any of the
documents.
- A function stop_word that is part of (last step of) the training process that identifies stop
words and removes the stop words from the hash table and adjusts the hash table and lists
accordingly.
- A main function that first calls the training process to read all the documents and create the
hash table. 

Uses: 
## tf-idf Algorithm
Givens: hashmap of documents, query of *m* words

Using results below, the *tf-idf* of a word *w* in document *i* is *tf-idf(w,i)=tf x idf*

For each document, the rank or relevance of the document is given by the sum of *tf-idf* scores for each word in the query.

### Term Frequency: tf
Frequency of word *w* in document *i* is given by *tf(w,i)*= (num_occurences of *w* in *i*)

###Inverse Document Frequency
-Document frequency *df* is the number of documents containing the term *w*
-Inverse Document Frequency *idf* is defined as *idf = log(N/df)* where *N* is the total number of documents (if *df*=0 then 1 is added to avoid N/0) so *idf = log(N/df+1)*


