### Sequential version
# FROM gcc:4.9
# COPY . /ProjectWork
# WORKDIR /ProjectWork
# RUN gcc -Wall -o seq sequential/sequential.c
# CMD ["./seq", "got/got_tokenized_sentences.txt", "0"]

### Parallel version
FROM gcc:4.9
COPY . /ProjectWork
WORKDIR /ProjectWork
RUN gcc -Wall -o para parallel/parallel_v1.c
CMD ["./para", "got/got_tokenized_sentences.txt", "0"]


### Commands for building and running the container
#docker build -t gcc-docker .
#docker run -it gcc-docker:latest