FROM ubuntu:latest

RUN apt-get update -y && \
    apt-get install -y \
    g++ \
    sqlite3 \
    libsqlite3-dev

COPY . /app

WORKDIR /app

RUN g++ -o /app/animalSimulator *.cpp -lsqlite3

CMD ["./animalSimulator"]