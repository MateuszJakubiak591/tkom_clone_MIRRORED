FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
   build-essential \
   cmake \
   gdb \
   && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cmake -S . -B build
RUN cmake --build build

CMD ["ctest", "--test-dir", "build", "--output-on-failure"]

# docker build -t djm-interpreter .
# docker run --rm djm-interpreter
# docker run --rm djm-interpreter ./build/djm ./src/program.djm