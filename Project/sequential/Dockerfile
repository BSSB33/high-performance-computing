FROM gcc:4.9
COPY . /ProjectWork
WORKDIR /ProjectWork
RUN gcc -Wall -o sequential sequential.c
CMD ["./sequential", "got_sentences.txt", "1"]


#docker build -t gcc-docker .
#docker run -it gcc-docker:latest