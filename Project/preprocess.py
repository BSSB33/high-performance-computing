import sys
import os
import re
import string
from nltk.corpus import stopwords
import nltk
from nltk.tokenize import word_tokenize
import time

#TODO - stopwords
#TODO - wrong word separation

def import_book(path):
    f = open(path, "r", encoding="utf-8")
    return f.read().replace(". ", ".").replace("\n", " ").replace("\r", "").replace("   ", " ")#.replace("    ", "")

def export_sentences(path, sentences):
    f = open(path, "w", encoding="utf-8")
    for sentence in sentences:
        f.write(str(sentence) + "\n")

def process_sentence(sentences):
    processed_sentences = []
    for sentence in sentences:
        sentence = sentence.replace("-", " ")
        sentence = re.sub('^\s*$', '', sentence)
        sentence = re.sub('[' + string.punctuation + ']', '', sentence)
        sentence = re.sub('[“”’—]*', '', sentence)
        sentence = re.sub("^\s+|\s+$", '', sentence, flags=re.UNICODE)
        processed_sentences.append(sentence)
    return processed_sentences

def tokenize_sentences(sentences):
    tokenized_sentences = []

    for sentence in sentences:
        tokenized_sentence = word_tokenize(sentence)
        for word in tokenized_sentence:
            tokenized_sentences.append(word)
    return tokenized_sentences

if __name__ == "__main__":
    nltk.download('punkt')

    start_time = time.time()

    path = sys.argv[1]
    book = import_book(path)
    path = path.replace(".txt", "")
    
    sentences = book.split(".")
    sentences = process_sentence(sentences)
    tokenized_sentences = tokenize_sentences(sentences)
    
    export_sentences(path + "_sentences.txt", sentences)
    export_sentences(path + "_tokenized_sentences.txt", tokenized_sentences)

    print("--- %s seconds ---" % (time.time() - start_time))