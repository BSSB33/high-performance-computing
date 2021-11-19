import sys
import re
import string
from nltk.corpus import stopwords
import nltk
from nltk.tokenize import word_tokenize
import time

#TODO - stopwords

def import_book(path):
    f = open(path, "r", encoding="utf-8")
    return f.read().replace(" . . .", " ").replace(". ", ".").replace("\n", " ").replace("\r", "").replace("   ", " ")

def export_sentences(path, sentences):
    f = open(path, "w", encoding="utf-8")
    for sentence in sentences:
        f.write(str(sentence) + "\n")

def process_sentence(sentences):
    processed_sentences = []
    for sentence in sentences:
        sentence = sentence.replace("-", " ")                           #Removing normal dash-es
        sentence = sentence.replace(" . . . ", " ")                     #Removing special characters
        sentence = re.sub('^\s*$', '', sentence)                        #Removing empty lines
        sentence = re.sub('[' + string.punctuation + ']', '', sentence) #Removing punctuation
        sentence = re.sub('[“”’—]*', '', sentence)                      #Removing special characters
        sentence = re.sub("^\s+|\s+$", '', sentence, flags=re.UNICODE)  #Removing whitespaces at the beginning and end of the sentence
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