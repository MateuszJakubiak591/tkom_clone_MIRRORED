FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
   build-essential \
   cmake \
   git \
   gdb \
   && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build --parallel

CMD ["ctest", "--test-dir", "build", "--output-on-failure"]
