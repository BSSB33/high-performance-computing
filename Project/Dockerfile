FROM gcc:4.9
COPY . /ProjectWork
WORKDIR /ProjectWork
RUN gcc -Wall -o seq sequential/sequential.c
CMD ["./seq", "got/got_tokenized_sentences.txt", "0"]


#docker build -t gcc-docker .
#docker run -it gcc-docker:latest