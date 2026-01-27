FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y libmysqlcppconn-dev libssl-dev

COPY src/ ./src/

RUN g++ src/*.cpp -o client -lmysqlcppconn -lssl -lcrypto

CMD ["./client"]