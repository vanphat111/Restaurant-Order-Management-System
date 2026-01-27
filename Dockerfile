FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y libmysqlcppconn-dev

COPY src/ ./src/

RUN g++ src/*.cpp -o client -lmysqlcppconn

CMD ["./client"]