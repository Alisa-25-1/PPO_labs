-- Удаляем старые таблицы если существуют
DROP TABLE IF EXISTS reviews CASCADE;
DROP TABLE IF EXISTS enrollments CASCADE;
DROP TABLE IF EXISTS lessons CASCADE;
DROP TABLE IF EXISTS trainer_specializations CASCADE;
DROP TABLE IF EXISTS trainers CASCADE;
DROP TABLE IF EXISTS bookings CASCADE;
DROP TABLE IF EXISTS subscriptions CASCADE;
DROP TABLE IF EXISTS subscription_types CASCADE;
DROP TABLE IF EXISTS clients CASCADE;
DROP TABLE IF EXISTS dance_halls CASCADE;
DROP TABLE IF EXISTS branches CASCADE;
DROP TABLE IF EXISTS studios CASCADE;

-- Расширение для UUID
CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Таблица студий
CREATE TABLE studios (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    name VARCHAR(255) NOT NULL,
    description TEXT,
    contact_email VARCHAR(255) NOT NULL
);

-- Таблица филиалов - ИСПРАВЛЕНО: используем INTEGER для часов
CREATE TABLE branches (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    name VARCHAR(255) NOT NULL,
    address TEXT NOT NULL,
    phone VARCHAR(50) NOT NULL,
    open_time INTEGER NOT NULL CHECK (open_time >= 0 AND open_time <= 23),
    close_time INTEGER NOT NULL CHECK (close_time >= 0 AND close_time <= 23),
    studio_id UUID NOT NULL REFERENCES studios(id)
);

-- Таблица залов
CREATE TABLE dance_halls (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    name VARCHAR(255) NOT NULL,
    description TEXT,
    capacity INTEGER NOT NULL CHECK (capacity > 0),
    floor_type VARCHAR(100) NOT NULL,
    equipment TEXT,
    branch_id UUID NOT NULL REFERENCES branches(id)
);

-- Таблица клиентов
CREATE TABLE clients (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    name VARCHAR(255) NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    phone VARCHAR(50) NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    registration_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    status VARCHAR(20) NOT NULL DEFAULT 'ACTIVE' CHECK (status IN ('ACTIVE', 'INACTIVE', 'SUSPENDED'))
);

-- Таблица преподавателей
CREATE TABLE trainers (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    name VARCHAR(255) NOT NULL,
    biography TEXT,
    qualification_level VARCHAR(50) NOT NULL,
    is_active BOOLEAN NOT NULL DEFAULT true
);

-- Таблица специализаций тренеров
CREATE TABLE trainer_specializations (
    trainer_id UUID NOT NULL REFERENCES trainers(id),
    specialization VARCHAR(100) NOT NULL,
    PRIMARY KEY (trainer_id, specialization)
);

-- Таблица типов абонементов
CREATE TABLE subscription_types (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    name VARCHAR(255) NOT NULL,
    description TEXT,
    validity_days INTEGER NOT NULL CHECK (validity_days > 0),
    visit_count INTEGER NOT NULL CHECK (visit_count >= 0),
    unlimited BOOLEAN NOT NULL DEFAULT false,
    price DECIMAL(10,2) NOT NULL CHECK (price >= 0)
);

-- Таблица абонементов
CREATE TABLE subscriptions (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    client_id UUID NOT NULL REFERENCES clients(id),
    subscription_type_id UUID NOT NULL REFERENCES subscription_types(id),
    start_date TIMESTAMP NOT NULL,
    end_date TIMESTAMP NOT NULL,
    remaining_visits INTEGER NOT NULL,
    status VARCHAR(20) NOT NULL DEFAULT 'ACTIVE' CHECK (status IN ('ACTIVE', 'SUSPENDED', 'EXPIRED', 'CANCELLED')),
    purchase_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- Таблица занятий
CREATE TABLE lessons (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    type VARCHAR(50) NOT NULL CHECK (type IN ('OPEN_CLASS', 'SPECIAL_COURSE', 'INDIVIDUAL', 'MASTERCLASS')),
    name VARCHAR(255) NOT NULL,
    description TEXT,
    start_time TIMESTAMP NOT NULL,
    duration_minutes INTEGER NOT NULL CHECK (duration_minutes > 0),
    difficulty VARCHAR(50) NOT NULL CHECK (difficulty IN ('BEGINNER', 'INTERMEDIATE', 'ADVANCED', 'ALL_LEVELS')),
    max_participants INTEGER NOT NULL CHECK (max_participants > 0),
    current_participants INTEGER NOT NULL DEFAULT 0 CHECK (current_participants >= 0),
    price DECIMAL(10,2) NOT NULL CHECK (price >= 0),
    status VARCHAR(50) NOT NULL DEFAULT 'SCHEDULED' CHECK (status IN ('SCHEDULED', 'ONGOING', 'COMPLETED', 'CANCELLED')),
    trainer_id UUID NOT NULL REFERENCES trainers(id),
    hall_id UUID NOT NULL REFERENCES dance_halls(id)
);

-- Таблица бронирований
CREATE TABLE bookings (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    client_id UUID NOT NULL REFERENCES clients(id),
    hall_id UUID NOT NULL REFERENCES dance_halls(id),
    start_time TIMESTAMP NOT NULL,
    duration_minutes INTEGER NOT NULL CHECK (duration_minutes > 0),
    purpose VARCHAR(255) NOT NULL,
    status VARCHAR(20) NOT NULL DEFAULT 'PENDING' CHECK (status IN ('PENDING', 'CONFIRMED', 'CANCELLED', 'COMPLETED')),
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- Таблица записей на занятия
CREATE TABLE enrollments (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    client_id UUID NOT NULL REFERENCES clients(id),
    lesson_id UUID NOT NULL REFERENCES lessons(id),
    status VARCHAR(20) NOT NULL DEFAULT 'REGISTERED' CHECK (status IN ('REGISTERED', 'CANCELLED', 'ATTENDED', 'MISSED')),
    enrollment_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(client_id, lesson_id)
);

-- Таблица отзывов
CREATE TABLE reviews (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    client_id UUID NOT NULL REFERENCES clients(id),
    lesson_id UUID NOT NULL REFERENCES lessons(id),
    rating INTEGER NOT NULL CHECK (rating >= 1 AND rating <= 5),
    comment TEXT,
    publication_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    status VARCHAR(20) NOT NULL DEFAULT 'PENDING_MODERATION' CHECK (status IN ('PENDING_MODERATION', 'APPROVED', 'REJECTED')),
    UNIQUE(client_id, lesson_id)
);

-- Таблица для отслеживания посещаемости
CREATE TABLE IF NOT EXISTS attendance (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    client_id UUID NOT NULL REFERENCES clients(id) ON DELETE CASCADE,
    entity_id UUID NOT NULL, -- ID занятия или бронирования
    type VARCHAR(20) NOT NULL CHECK (type IN ('LESSON', 'BOOKING')),
    status VARCHAR(20) NOT NULL CHECK (status IN ('SCHEDULED', 'VISITED', 'CANCELLED', 'NO_SHOW')),
    scheduled_time TIMESTAMP NOT NULL,
    actual_time TIMESTAMP NOT NULL,
    notes TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    INDEX idx_attendance_client_id (client_id),
    INDEX idx_attendance_entity_id (entity_id),
    INDEX idx_attendance_type_status (type, status),
    INDEX idx_attendance_scheduled_time (scheduled_time)
);

-- Индексы для улучшения производительности
CREATE INDEX idx_bookings_client_id ON bookings(client_id);
CREATE INDEX idx_bookings_hall_id ON bookings(hall_id);
CREATE INDEX idx_bookings_start_time ON bookings(start_time);
CREATE INDEX idx_clients_email ON clients(email);
CREATE INDEX idx_subscriptions_client_id ON subscriptions(client_id);
CREATE INDEX idx_lessons_trainer_id ON lessons(trainer_id);
CREATE INDEX idx_lessons_hall_id ON lessons(hall_id);
CREATE INDEX idx_lessons_start_time ON lessons(start_time);
CREATE INDEX idx_lessons_status ON lessons(status);
CREATE INDEX idx_enrollments_client_id ON enrollments(client_id);
CREATE INDEX idx_enrollments_lesson_id ON enrollments(lesson_id);
CREATE INDEX idx_enrollments_status ON enrollments(status);
CREATE INDEX idx_reviews_status ON reviews(status);
CREATE INDEX idx_reviews_rating ON reviews(rating);
CREATE INDEX idx_trainers_active ON trainers(is_active);

-- Индекс для быстрого поиска по клиенту и статусу
CREATE INDEX IF NOT EXISTS idx_attendance_client_status ON attendance(client_id, status);

-- Индекс для статистических запросов
CREATE INDEX IF NOT EXISTS idx_attendance_type_client ON attendance(type, client_id, status);

-- Права доступа
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO dance_user;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO dance_user;