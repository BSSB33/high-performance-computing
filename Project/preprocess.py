import sys
import re
import string
import time
from nltk import stem
from nltk.corpus import stopwords
from nltk.stem import PorterStemmer
from nltk.tokenize import word_tokenize
import nltk

def import_book(path):
    f = open(path, "r", encoding="utf-8")
    book = f.read().replace(" . . .", " ").replace(". ", ".").replace("\n", " ").replace("\r", "").replace("   ", " ")
    f.close()
    return book

def export_sentences(path, sentences):
    f = open(path, "w", encoding="utf-8")
    f.write(str(len(sentences)) + "\n")
    for sentence in sentences:
        f.write(str(sentence) + "\n")

def process_sentence(sentences):
    processed_sentences = []
    for sentence in sentences:
        sentence = sentence.replace("-", " ")                           #Removing normal dash-es
        sentence = sentence.replace(" . . . ", " ").replace("...", " ") #Removing special characters  
        sentence = re.sub('^\s*$', '', sentence)                        #Removing empty lines
        sentence = re.sub('[' + string.punctuation + ']', '', sentence) #Removing punctuation
        sentence = re.sub('[“”’—\\|\/]*', '', sentence)                 #Removing special characters
        sentence = re.sub("^\s+|\s+$", '', sentence, flags=re.UNICODE)  #Removing whitespaces at the beginning and end of the sentence
        if(sentence != ""):
            processed_sentences.append(sentence)
    return processed_sentences

def tokenize_sentences(sentences, stemming=False):
    tokenized_sentences = []
    stopwords_english = stopwords.words('english')
    conjunctions  = ["I", "so", "a", "the", "to", "of", "his", "her", "was", "in", "had", "you", "he", "she", "it", "is", "as", "with", "be", "have", "at", "on"]
    stemmer = PorterStemmer()

    for sentence in sentences:
        tokenized_sentence = word_tokenize(sentence)
        for word in tokenized_sentence:
            if(stemming): #If Stemming is enabled
                word = stemmer.stem(word)
            if(word not in stopwords_english and word not in conjunctions and word not in string.punctuation): #Removing stopwords and punctuation
                tokenized_sentences.append(word)
    return tokenized_sentences

if __name__ == "__main__":

    stemming = True
    if(len(sys.argv) == 3):
        print("Generating Outputs...")
        stemming = sys.argv[2]
        if (stemming == 'yes' or stemming == 'y'): 
            stemming = True
            print("Using Stemming...")
        elif(stemming == 'no' or stemming == 'n'): 
            print("Avoiding Stemming...")
            stemming = False
    elif(len(sys.argv) == 2): print("Using Stemming...")

    elif(len(sys.argv) in [0, 1] or len(sys.argv) > 3):
        print("Usage: py preprocess.py <path_to_file> yes/no (If Stemming is required)")
        sys.exit()

    nltk.download('punkt')
    nltk.download('stopwords')

    start_time = time.time()

    path = sys.argv[1]
    book = import_book(path)
    path = path.replace(".txt", "")
    
    sentences = book.split(".")
    sentences = process_sentence(sentences)
    tokenized_sentences = tokenize_sentences(sentences, stemming)
    
    export_sentences(path + "_sentences.txt", sentences)
    export_sentences(path + "_tokenized_sentences.txt", tokenized_sentences)

    print("--- %s seconds ---" % (time.time() - start_time))