-- Создание таблицы клиентов
CREATE TABLE IF NOT EXISTS clients (
    id UUID PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    phone VARCHAR(50) NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    registration_date TIMESTAMP NOT NULL,
    status VARCHAR(20) NOT NULL CHECK (status IN ('ACTIVE', 'INACTIVE', 'SUSPENDED'))
);

-- Создание таблицы залов
CREATE TABLE IF NOT EXISTS halls (
    id UUID PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    description TEXT,
    capacity INTEGER NOT NULL CHECK (capacity > 0),
    floor_type VARCHAR(100) NOT NULL,
    equipment TEXT,
    branch_id UUID NOT NULL
);

-- Создание таблицы бронирований
CREATE TABLE IF NOT EXISTS bookings (
    id UUID PRIMARY KEY,
    client_id UUID NOT NULL REFERENCES clients(id),
    hall_id UUID NOT NULL REFERENCES halls(id),
    start_time TIMESTAMP NOT NULL,
    duration_minutes INTEGER NOT NULL CHECK (duration_minutes > 0),
    purpose VARCHAR(255) NOT NULL,
    status VARCHAR(20) NOT NULL CHECK (status IN ('PENDING', 'CONFIRMED', 'CANCELLED', 'COMPLETED')),
    created_at TIMESTAMP NOT NULL
);

-- Индексы для улучшения производительности
CREATE INDEX IF NOT EXISTS idx_bookings_client_id ON bookings(client_id);
CREATE INDEX IF NOT EXISTS idx_bookings_hall_id ON bookings(hall_id);
CREATE INDEX IF NOT EXISTS idx_bookings_start_time ON bookings(start_time);
CREATE INDEX IF NOT EXISTS idx_clients_email ON clients(email);

GRANT ALL PRIVILEGES ON TABLE clients, halls, bookings TO dance_user;
GRANT USAGE, SELECT ON ALL SEQUENCES IN SCHEMA public TO dance_user;