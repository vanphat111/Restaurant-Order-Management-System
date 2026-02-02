FROM gcc:latest

WORKDIR /app

RUN apt-get update && apt-get install -y libmysqlcppconn-dev libssl-dev cmake

COPY src/ ./src/
COPY CMakeLists.txt .

WORKDIR /app/build

RUN cmake ..

RUN make

CMD ["./nha-hang-client"]