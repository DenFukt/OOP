# ─────────────────────────────────────────────────────────────────
#  Stage 1: Збірка
# ─────────────────────────────────────────────────────────────────
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    g++ gcc make wget curl ca-certificates \
    --no-install-recommends && rm -rf /var/lib/apt/lists/*

WORKDIR /build

# Копіюємо весь проєкт
COPY . .

# Завантажуємо httplib.h (header-only HTTP сервер)
RUN wget -q -O httplib.h \
    "https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h"

# Компілюємо SQLite окремо
RUN gcc -c sqlite3.c -o sqlite3.o

# Збираємо головну програму
RUN g++ -std=c++17 -I. -pthread \
    main.cpp sqlite3.o \
    -o lab5 \
    -ldl -lpthread

# ─────────────────────────────────────────────────────────────────
#  Stage 2: Runtime (мінімальний образ)
# ─────────────────────────────────────────────────────────────────
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    ca-certificates curl \
    --no-install-recommends && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Копіюємо скомпільований бінарник і фронтенд
COPY --from=builder /build/lab5     ./lab5
COPY frontend/                      ./frontend/

# Порт, що відкриває HTTP сервер
EXPOSE 8080

# Health check для Docker
HEALTHCHECK --interval=15s --timeout=5s --start-period=5s --retries=3 \
    CMD curl -sf http://localhost:8080/api/health || exit 1

# Змінна середовища для Firebase (опціонально)
ENV FIREBASE_URL=""

CMD ["./lab5", "8080"]
