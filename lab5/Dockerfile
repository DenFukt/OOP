FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    g++ gcc make wget curl ca-certificates \
    --no-install-recommends && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .

RUN wget -q -O httplib.h \
    "https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h"

RUN gcc -c sqlite3.c -o sqlite3.o

RUN g++ -std=c++17 -I. -pthread \
    main.cpp sqlite3.o -o lab5 -ldl -lpthread

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    ca-certificates curl \
    --no-install-recommends && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /build/lab5 ./lab5
COPY frontend/ ./frontend/

EXPOSE 8080

HEALTHCHECK --interval=15s --timeout=5s --retries=3 \
    CMD curl -sf http://localhost:${PORT:-8080}/api/health || exit 1

ENV FIREBASE_URL=""

CMD ["./lab5"]
