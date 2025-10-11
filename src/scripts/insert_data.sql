-- init_test_data.sql
-- Скрипт для заполнения тестовыми данными

-- Очистка существующих данных (осторожно!)
DELETE FROM reviews;
DELETE FROM enrollments;
DELETE FROM lessons;
DELETE FROM trainer_specializations;
DELETE FROM trainers;
DELETE FROM bookings;
DELETE FROM subscriptions;
DELETE FROM subscription_types;
DELETE FROM clients;
DELETE FROM dance_halls;
DELETE FROM branches;
DELETE FROM studios;

-- 1. Создание студии
INSERT INTO studios (id, name, description, contact_email) VALUES
('11111111-1111-1111-1111-111111111111', 'Танцевальная студия "Грация"', 'Лучшая студия танцев в городе с профессиональными преподавателями', 'info@gracia-dance.ru');

-- 2. Создание филиала
INSERT INTO branches (id, name, address, phone, open_time, close_time, studio_id) VALUES
('22222222-2222-2222-2222-222222222222', 'Центральный филиал', 'ул. Центральная, д. 1', '+7-495-123-45-67', 9, 22, '11111111-1111-1111-1111-111111111111');

-- 3. Создание танцевальных залов
INSERT INTO dance_halls (id, name, description, capacity, floor_type, equipment, branch_id) VALUES
('33333333-3333-3333-3333-333333333333', 'Зал А', 'Просторный зал с естественным освещением', 30, 'wooden', 'зеркала, станки, музыкальная система', '22222222-2222-2222-2222-222222222222'),
('44444444-4444-4444-4444-444444444444', 'Зал Б', 'Зал для индивидуальных занятий', 10, 'marley', 'зеркала, кондиционер', '22222222-2222-2222-2222-222222222222'),
('55555555-5555-5555-5555-555555555555', 'Зал В', 'Большой зал для групповых занятий', 50, 'vinyl', 'зеркала, станки, проектор', '22222222-2222-2222-2222-222222222222');

-- 4. Создание клиентов
INSERT INTO clients (id, name, email, phone, password_hash, registration_date, status) VALUES
('66666666-6666-6666-6666-666666666666', 'Иванова Мария', 'ivanova@example.com', '+7-915-123-45-67', 'hashed_password_1', CURRENT_TIMESTAMP, 'ACTIVE'),
('77777777-7777-7777-7777-777777777777', 'Петров Алексей', 'petrov@example.com', '+7-916-234-56-78', 'hashed_password_2', CURRENT_TIMESTAMP, 'ACTIVE'),
('88888888-8888-8888-8888-888888888888', 'Сидорова Анна', 'sidorova@example.com', '+7-917-345-67-89', 'hashed_password_3', CURRENT_TIMESTAMP, 'ACTIVE');

-- 5. Создание преподавателей
INSERT INTO trainers (id, name, biography, qualification_level, is_active) VALUES
('99999999-9999-9999-9999-999999999999', 'Смирнова Ольга', 'Профессиональная балерина с 10-летним опытом преподавания', 'senior', true),
('aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa', 'Кузнецов Дмитрий', 'Специалист по современным танцевальным направлениям', 'master', true),
('bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbbbbbb', 'Васильева Елена', 'Эксперт по латиноамериканским танцам', 'senior', true);

-- 6. Создание специализаций преподавателей
INSERT INTO trainer_specializations (trainer_id, specialization) VALUES
('99999999-9999-9999-9999-999999999999', 'балет'),
('99999999-9999-9999-9999-999999999999', 'классический танец'),
('aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa', 'хип-хоп'),
('aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa', 'джаз-фанк'),
('bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbbbbbb', 'сальса'),
('bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbbbbbb', 'бачата');

-- 7. Создание типов абонементов
INSERT INTO subscription_types (id, name, description, validity_days, visit_count, unlimited, price) VALUES
('cccccccc-cccc-cccc-cccc-cccccccccccc', 'Разовый', 'Одно посещение любого занятия', 1, 1, false, 500.00),
('dddddddd-dddd-dddd-dddd-dddddddddddd', 'Месячный (8 занятий)', '8 занятий в течение 30 дней', 30, 8, false, 3000.00),
('eeeeeeee-eeee-eeee-eeee-eeeeeeeeeeee', 'Месячный безлимитный', 'Неограниченное количество занятий в течение 30 дней', 30, 0, true, 5000.00),
('ffffffff-ffff-ffff-ffff-ffffffffffff', 'Пробный', '3 занятия для новых клиентов', 14, 3, false, 1000.00);

-- 8. Создание абонементов
INSERT INTO subscriptions (id, client_id, subscription_type_id, start_date, end_date, remaining_visits, status, purchase_date) VALUES
('11111111-1111-1111-1111-111111111112', '66666666-6666-6666-6666-666666666666', 'eeeeeeee-eeee-eeee-eeee-eeeeeeeeeeee', CURRENT_DATE, CURRENT_DATE + INTERVAL '30 days', -1, 'ACTIVE', CURRENT_TIMESTAMP),
('11111111-1111-1111-1111-111111111113', '77777777-7777-7777-7777-777777777777', 'dddddddd-dddd-dddd-dddd-dddddddddddd', CURRENT_DATE, CURRENT_DATE + INTERVAL '30 days', 8, 'ACTIVE', CURRENT_TIMESTAMP);

-- 9. Создание занятий
INSERT INTO lessons (id, type, name, description, start_time, duration_minutes, difficulty, max_participants, current_participants, price, status, trainer_id, hall_id) VALUES
('22222222-2222-2222-2222-222222222223', 'OPEN_CLASS', 'Балет для начинающих', 'Основы классического танца для новичков', CURRENT_TIMESTAMP + INTERVAL '2 days', 60, 'BEGINNER', 15, 0, 500.00, 'SCHEDULED', '99999999-9999-9999-9999-999999999999', '33333333-3333-3333-3333-333333333333'),
('22222222-2222-2222-2222-222222222224', 'SPECIAL_COURSE', 'Хип-хоп продвинутый', 'Продвинутая техника хип-хопа', CURRENT_TIMESTAMP + INTERVAL '3 days', 90, 'ADVANCED', 20, 0, 700.00, 'SCHEDULED', 'aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa', '55555555-5555-5555-5555-555555555555'),
('22222222-2222-2222-2222-222222222225', 'MASTERCLASS', 'Сальса мастер-класс', 'Мастер-класс от профессионального танцора', CURRENT_TIMESTAMP + INTERVAL '5 days', 120, 'INTERMEDIATE', 25, 0, 1000.00, 'SCHEDULED', 'bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbbbbbb', '55555555-5555-5555-5555-555555555555'),
('22222222-2222-2222-2222-222222222226', 'INDIVIDUAL', 'Индивидуальное занятие', 'Персональная тренировка', CURRENT_TIMESTAMP + INTERVAL '1 day', 60, 'ALL_LEVELS', 1, 0, 1500.00, 'SCHEDULED', '99999999-9999-9999-9999-999999999999', '44444444-4444-4444-4444-444444444444');

-- 10. Создание бронирований
INSERT INTO bookings (id, client_id, hall_id, start_time, duration_minutes, purpose, status, created_at) VALUES
('33333333-3333-3333-3333-333333333334', '66666666-6666-6666-6666-666666666666', '44444444-4444-4444-4444-444444444444', CURRENT_TIMESTAMP + INTERVAL '1 day', 120, 'Индивидуальная репетиция', 'CONFIRMED', CURRENT_TIMESTAMP),
('33333333-3333-3333-3333-333333333335', '77777777-7777-7777-7777-777777777777', '33333333-3333-3333-3333-333333333333', CURRENT_TIMESTAMP + INTERVAL '4 days', 90, 'Танцевальная практика с друзьями', 'PENDING', CURRENT_TIMESTAMP);

-- 11. Создание записей на занятия
INSERT INTO enrollments (id, client_id, lesson_id, status, enrollment_date) VALUES
('44444444-4444-4444-4444-444444444445', '66666666-6666-6666-6666-666666666666', '22222222-2222-2222-2222-222222222223', 'REGISTERED', CURRENT_TIMESTAMP),
('44444444-4444-4444-4444-444444444446', '77777777-7777-7777-7777-777777777777', '22222222-2222-2222-2222-222222222224', 'REGISTERED', CURRENT_TIMESTAMP),
('44444444-4444-4444-4444-444444444447', '88888888-8888-8888-8888-888888888888', '22222222-2222-2222-2222-222222222225', 'REGISTERED', CURRENT_TIMESTAMP);

-- 12. Создание отзывов (для завершенных занятий)
-- Сначала создадим завершенное занятие для отзывов
INSERT INTO lessons (id, type, name, description, start_time, duration_minutes, difficulty, max_participants, current_participants, price, status, trainer_id, hall_id) VALUES
('22222222-2222-2222-2222-222222222227', 'OPEN_CLASS', 'Базовый балет', 'Вводное занятие по балету', CURRENT_TIMESTAMP - INTERVAL '5 days', 60, 'BEGINNER', 15, 10, 500.00, 'COMPLETED', '99999999-9999-9999-9999-999999999999', '33333333-3333-3333-3333-333333333333');

-- Запись на завершенное занятие
INSERT INTO enrollments (id, client_id, lesson_id, status, enrollment_date) VALUES
('44444444-4444-4444-4444-444444444448', '66666666-6666-6666-6666-666666666666', '22222222-2222-2222-2222-222222222227', 'ATTENDED', CURRENT_TIMESTAMP - INTERVAL '6 days');

-- Отзывы
INSERT INTO reviews (id, client_id, lesson_id, rating, comment, publication_date, status) VALUES
('55555555-5555-5555-5555-555555555556', '66666666-6666-6666-6666-666666666666', '22222222-2222-2222-2222-222222222227', 5, 'Отличное занятие! Преподаватель очень внимательный и профессиональный.', CURRENT_TIMESTAMP - INTERVAL '4 days', 'APPROVED');

-- Вывод информации о добавленных данных
SELECT '✅ Тестовые данные успешно добавлены!' as message;

-- Показать добавленные данные
SELECT 'СТУДИИ:' as info; SELECT * FROM studios;
SELECT 'ФИЛИАЛЫ:' as info; SELECT * FROM branches;
SELECT 'ЗАЛЫ:' as info; SELECT * FROM dance_halls;
SELECT 'КЛИЕНТЫ:' as info; SELECT * FROM clients;
SELECT 'ПРЕПОДАВАТЕЛИ:' as info; SELECT * FROM trainers;
SELECT 'СПЕЦИАЛИЗАЦИИ:' as info; SELECT * FROM trainer_specializations;
SELECT 'ТИПЫ АБОНЕМЕНТОВ:' as info; SELECT * FROM subscription_types;
SELECT 'АБОНЕМЕНТЫ:' as info; SELECT * FROM subscriptions;
SELECT 'ЗАНЯТИЯ:' as info; SELECT * FROM lessons;
SELECT 'БРОНИРОВАНИЯ:' as info; SELECT * FROM bookings;
SELECT 'ЗАПИСИ НА ЗАНЯТИЯ:' as info; SELECT * FROM enrollments;
SELECT 'ОТЗЫВЫ:' as info; SELECT * FROM reviews;