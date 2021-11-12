import re
import string
from nltk.corpus import stopwords
from nltk.stem import PorterStemmer
from nltk.tokenize import TweetTokenizer

def import_book(path):
    f = open(path, "r", encoding="utf8")
    return f.read().replace(". ", ".").replace("    ", "").replace("    ", "").replace("\n", "").replace("\r", "")

def export_sentences(path, sentences):
    f = open(path, "w", encoding="utf8")
    for sentence in sentences:
        f.write(str(sentence) + "\n")

def process_sentence(sentences):
    processed_sentences = []
    for sentence in sentences:
        # Remove punctuation and extra characters
        sentence = re.sub('[' + string.punctuation + ']', '', sentence)
        sentence = re.sub('[“”]*', '', sentence)
        sentence = re.sub("^\s+|\s+$", "", sentence, flags=re.UNICODE)
        processed_sentences.append(sentence)
    return processed_sentences

def tokenize_sentences(sentences):
    tokenized_sentences = []
    tokenizer = TweetTokenizer(preserve_case=False, strip_handles=True, reduce_len=True)

    for sentence in sentences:
        sentence_tokens = tokenizer.tokenize(sentence)
        tokenized_sentences.append(sentence_tokens)
    return tokenized_sentences

if __name__ == "__main__":
    book = import_book("alice.txt")
    sentences = book.split(".")
    sentences = process_sentence(sentences)
    tokenized_sentences = tokenize_sentences(sentences)
    export_sentences("alice_sentences.txt", sentences)
    export_sentences("alice_tokenized_sentences.txt", tokenized_sentences)