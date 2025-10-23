# Dockerfile
FROM ubuntu:22.04 as builder

# Устанавливаем зависимости для сборки
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libpq-dev \
    libpqxx-dev \
    pkg-config \
    postgresql-server-dev-all \
    libgtest-dev \
    google-mock \
    && rm -rf /var/lib/apt/lists/*

# Устанавливаем uuid-dev
RUN apt-get update && apt-get install -y uuid-dev

# Копируем исходный код
WORKDIR /app
COPY . .

# Создаем директорию для сборки и собираем
RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)

# Финальный образ
FROM ubuntu:22.04

# Устанавливаем рантайм зависимости
RUN apt-get update && apt-get install -y \
    libpq5 \
    libpqxx-6.4 \
    libgtest-dev \
    libgmock-dev \
    && rm -rf /var/lib/apt/lists/*

# Создаем пользователя для безопасности
RUN useradd -m -u 1000 appuser
USER appuser
WORKDIR /home/appuser

# Копируем собранное приложение
COPY --from=builder /app/build/TechUI /home/appuser/
COPY --from=builder /app/config.json /home/appuser/config.json
COPY --from=builder /app/scripts/init_database.sql /home/appuser/scripts/

# Создаем директории для логов
RUN mkdir -p /home/appuser/logs

# Указываем точку входа
CMD ["./TechUI"]