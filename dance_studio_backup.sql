--
-- PostgreSQL database dump
--

\restrict sjkgdCnWWNPSIChp9Mfko0ahF3iLEkOea56RsyuHqVjQIBxQHDfJRTHKwR1Yz4g

-- Dumped from database version 18.0 (Ubuntu 18.0-1.pgdg24.04+3)
-- Dumped by pg_dump version 18.0 (Ubuntu 18.0-1.pgdg24.04+3)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET transaction_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: uuid-ossp; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS "uuid-ossp" WITH SCHEMA public;


--
-- Name: EXTENSION "uuid-ossp"; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION "uuid-ossp" IS 'generate universally unique identifiers (UUIDs)';


SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: bookings; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.bookings (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    client_id uuid NOT NULL,
    hall_id uuid NOT NULL,
    start_time timestamp without time zone NOT NULL,
    duration_minutes integer NOT NULL,
    purpose character varying(255) NOT NULL,
    status character varying(20) DEFAULT 'PENDING'::character varying NOT NULL,
    created_at timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    CONSTRAINT bookings_duration_minutes_check CHECK ((duration_minutes > 0)),
    CONSTRAINT bookings_status_check CHECK (((status)::text = ANY ((ARRAY['PENDING'::character varying, 'CONFIRMED'::character varying, 'CANCELLED'::character varying, 'COMPLETED'::character varying])::text[])))
);


ALTER TABLE public.bookings OWNER TO postgres;

--
-- Name: branches; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.branches (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    name character varying(255) NOT NULL,
    address text NOT NULL,
    phone character varying(50) NOT NULL,
    open_time integer NOT NULL,
    close_time integer NOT NULL,
    studio_id uuid NOT NULL,
    CONSTRAINT branches_close_time_check CHECK (((close_time >= 0) AND (close_time <= 23))),
    CONSTRAINT branches_open_time_check CHECK (((open_time >= 0) AND (open_time <= 23)))
);


ALTER TABLE public.branches OWNER TO postgres;

--
-- Name: clients; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.clients (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    name character varying(255) NOT NULL,
    email character varying(255) NOT NULL,
    phone character varying(50) NOT NULL,
    password_hash character varying(255) NOT NULL,
    registration_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    status character varying(20) DEFAULT 'ACTIVE'::character varying NOT NULL,
    CONSTRAINT clients_status_check CHECK (((status)::text = ANY ((ARRAY['ACTIVE'::character varying, 'INACTIVE'::character varying, 'SUSPENDED'::character varying])::text[])))
);


ALTER TABLE public.clients OWNER TO postgres;

--
-- Name: dance_halls; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.dance_halls (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    name character varying(255) NOT NULL,
    description text,
    capacity integer NOT NULL,
    floor_type character varying(100) NOT NULL,
    equipment text,
    branch_id uuid NOT NULL,
    CONSTRAINT dance_halls_capacity_check CHECK ((capacity > 0))
);


ALTER TABLE public.dance_halls OWNER TO postgres;

--
-- Name: enrollments; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.enrollments (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    client_id uuid NOT NULL,
    lesson_id uuid NOT NULL,
    status character varying(20) DEFAULT 'REGISTERED'::character varying NOT NULL,
    enrollment_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    CONSTRAINT enrollments_status_check CHECK (((status)::text = ANY ((ARRAY['REGISTERED'::character varying, 'CANCELLED'::character varying, 'ATTENDED'::character varying, 'MISSED'::character varying])::text[])))
);


ALTER TABLE public.enrollments OWNER TO postgres;

--
-- Name: lessons; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.lessons (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    type character varying(50) NOT NULL,
    name character varying(255) NOT NULL,
    description text,
    start_time timestamp without time zone NOT NULL,
    duration_minutes integer NOT NULL,
    difficulty character varying(50) NOT NULL,
    max_participants integer NOT NULL,
    current_participants integer DEFAULT 0 NOT NULL,
    price numeric(10,2) NOT NULL,
    status character varying(50) DEFAULT 'SCHEDULED'::character varying NOT NULL,
    trainer_id uuid NOT NULL,
    hall_id uuid NOT NULL,
    CONSTRAINT lessons_current_participants_check CHECK ((current_participants >= 0)),
    CONSTRAINT lessons_difficulty_check CHECK (((difficulty)::text = ANY ((ARRAY['BEGINNER'::character varying, 'INTERMEDIATE'::character varying, 'ADVANCED'::character varying, 'ALL_LEVELS'::character varying])::text[]))),
    CONSTRAINT lessons_duration_minutes_check CHECK ((duration_minutes > 0)),
    CONSTRAINT lessons_max_participants_check CHECK ((max_participants > 0)),
    CONSTRAINT lessons_price_check CHECK ((price >= (0)::numeric)),
    CONSTRAINT lessons_status_check CHECK (((status)::text = ANY ((ARRAY['SCHEDULED'::character varying, 'ONGOING'::character varying, 'COMPLETED'::character varying, 'CANCELLED'::character varying])::text[]))),
    CONSTRAINT lessons_type_check CHECK (((type)::text = ANY ((ARRAY['OPEN_CLASS'::character varying, 'SPECIAL_COURSE'::character varying, 'INDIVIDUAL'::character varying, 'MASTERCLASS'::character varying])::text[])))
);


ALTER TABLE public.lessons OWNER TO postgres;

--
-- Name: reviews; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.reviews (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    client_id uuid NOT NULL,
    lesson_id uuid NOT NULL,
    rating integer NOT NULL,
    comment text,
    publication_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    status character varying(20) DEFAULT 'PENDING_MODERATION'::character varying NOT NULL,
    CONSTRAINT reviews_rating_check CHECK (((rating >= 1) AND (rating <= 5))),
    CONSTRAINT reviews_status_check CHECK (((status)::text = ANY ((ARRAY['PENDING_MODERATION'::character varying, 'APPROVED'::character varying, 'REJECTED'::character varying])::text[])))
);


ALTER TABLE public.reviews OWNER TO postgres;

--
-- Name: studios; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.studios (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    name character varying(255) NOT NULL,
    description text,
    contact_email character varying(255) NOT NULL
);


ALTER TABLE public.studios OWNER TO postgres;

--
-- Name: subscription_types; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.subscription_types (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    name character varying(255) NOT NULL,
    description text,
    validity_days integer NOT NULL,
    visit_count integer NOT NULL,
    unlimited boolean DEFAULT false NOT NULL,
    price numeric(10,2) NOT NULL,
    CONSTRAINT subscription_types_price_check CHECK ((price >= (0)::numeric)),
    CONSTRAINT subscription_types_validity_days_check CHECK ((validity_days > 0)),
    CONSTRAINT subscription_types_visit_count_check CHECK ((visit_count >= 0))
);


ALTER TABLE public.subscription_types OWNER TO postgres;

--
-- Name: subscriptions; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.subscriptions (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    client_id uuid NOT NULL,
    subscription_type_id uuid NOT NULL,
    start_date timestamp without time zone NOT NULL,
    end_date timestamp without time zone NOT NULL,
    remaining_visits integer NOT NULL,
    status character varying(20) DEFAULT 'ACTIVE'::character varying NOT NULL,
    purchase_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    CONSTRAINT subscriptions_status_check CHECK (((status)::text = ANY ((ARRAY['ACTIVE'::character varying, 'SUSPENDED'::character varying, 'EXPIRED'::character varying, 'CANCELLED'::character varying])::text[])))
);


ALTER TABLE public.subscriptions OWNER TO postgres;

--
-- Name: trainer_specializations; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.trainer_specializations (
    trainer_id uuid NOT NULL,
    specialization character varying(100) NOT NULL
);


ALTER TABLE public.trainer_specializations OWNER TO postgres;

--
-- Name: trainers; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.trainers (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    name character varying(255) NOT NULL,
    biography text,
    qualification_level character varying(50) NOT NULL,
    is_active boolean DEFAULT true NOT NULL
);


ALTER TABLE public.trainers OWNER TO postgres;

--
-- Data for Name: bookings; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.bookings (id, client_id, hall_id, start_time, duration_minutes, purpose, status, created_at) FROM stdin;
3a583573-c79f-492f-b6f9-139e9e9747dc	f3501785-5446-40c0-800e-9658d0399822	329629cb-b570-4e94-90de-67190b35815f	2025-10-17 15:00:00	60	indiv	CONFIRMED	2025-10-16 09:42:00
af274d5c-e5bc-4f99-a5e8-6dbe0030819c	f3501785-5446-40c0-800e-9658d0399822	78aaef0d-08e3-4ca3-87d7-d6be290843b9	2025-10-19 11:00:00	180	group	CONFIRMED	2025-10-16 09:42:30
f6a2a994-deea-434e-b93d-ac371ce602c6	d76c920d-4cf6-40aa-94e4-21b06292e160	329629cb-b570-4e94-90de-67190b35815f	2025-10-26 10:00:00	120	indiv	CONFIRMED	2025-10-23 07:55:09
706d32f4-cf67-46b0-bf5b-ff05594c4a17	4e2326e8-ae2a-4755-ae7f-3ddbffdf2aff	3d9d31de-9016-41a7-84b4-50ac0dd3a54e	2025-10-27 09:00:00	180	indiv	CANCELLED	2025-10-23 08:31:29
9cc176e5-bc12-4a2d-a4e4-6c7b8d65a2a8	d76c920d-4cf6-40aa-94e4-21b06292e160	329629cb-b570-4e94-90de-67190b35815f	2025-10-17 13:00:00	120	individual	CANCELLED	2025-10-16 09:39:51
7ea22011-fe5c-4081-9a2f-465df4d86df0	4762cb6f-8d76-459c-a43f-3edc48c54f1a	329629cb-b570-4e94-90de-67190b35815f	2025-10-27 08:00:00	180	Solo	CONFIRMED	2025-10-24 12:02:42
b707d949-35fd-47df-b6b1-de1b8179e3bc	d76c920d-4cf6-40aa-94e4-21b06292e160	329629cb-b570-4e94-90de-67190b35815f	2025-10-29 11:00:00	120	Постановка Соло	CONFIRMED	2025-10-24 12:33:55
e586225d-7037-43d7-9098-7f6e1e5d1934	d76c920d-4cf6-40aa-94e4-21b06292e160	3d9d31de-9016-41a7-84b4-50ac0dd3a54e	2025-10-18 09:00:00	60	dance group	CANCELLED	2025-10-16 09:40:22
71c69661-67d8-4f5a-be76-c3b075038e3a	d76c920d-4cf6-40aa-94e4-21b06292e160	78aaef0d-08e3-4ca3-87d7-d6be290843b9	2025-11-12 09:00:00	120	танцевать круто танец под музычку из тик-тока дыкындыш дыык дык дык	CONFIRMED	2025-10-24 15:21:54
\.


--
-- Data for Name: branches; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.branches (id, name, address, phone, open_time, close_time, studio_id) FROM stdin;
22222222-2222-2222-2222-222222222222	Центральный филиал	ул. Центральная, д. 1	+7-495-123-45-67	9	22	11111111-1111-1111-1111-111111111111
\.


--
-- Data for Name: clients; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.clients (id, name, email, phone, password_hash, registration_date, status) FROM stdin;
d76c920d-4cf6-40aa-94e4-21b06292e160	Shchukin Nik	shchukin-nik5110@yandex.ru	+79250386085	Password123	2025-10-11 10:59:58	ACTIVE
f4520709-9a0c-4ecb-a0d4-77eb367a3bca	Drygay fio	example@yandex.ru	+79035672435	Password321	2025-10-11 11:03:04	ACTIVE
f3501785-5446-40c0-800e-9658d0399822	Shchukin Nik 	anotheremail@yandex.ru	+79250386085	Password123	2025-10-15 22:00:35	ACTIVE
4d2c02d6-ddf6-4050-a5d3-a118929b5d0f	Shchukin Nik	ehwiuh@yandex.ru	+79037298307	Password123	2025-10-21 13:53:09	ACTIVE
aa67938b-cbaa-4c70-a785-3a40564ea2fd	Petrova Ksenya	petrova@gmail.com	+79255052590	Password123	2025-10-21 17:17:32	ACTIVE
5b75ade0-f1d9-4bb5-a6ec-9184866dab1f	Smirnov Maksim	smirnov@yandex.ru	+79251234567	Password123	2025-10-22 15:22:02	ACTIVE
4e2326e8-ae2a-4755-ae7f-3ddbffdf2aff	Kceniya Sobchak	sobchak@yandex.ru	+78005553535	Password123	2025-10-23 07:57:27	ACTIVE
df2f2d94-4a5a-43b8-8df4-b1b889039beb	Alexsandra Samsonova	samsonova@gmail.com	+79259006478	Password123	2025-10-23 11:28:31	ACTIVE
944a75e9-f864-4d1e-b750-8a75dd910eae	Васильев Павел	valilev@yandex.ru	+79256789543	Password123	2025-10-23 12:50:04	ACTIVE
4762cb6f-8d76-459c-a43f-3edc48c54f1a	Екатерина Мизулина	mizylina@gmail.com	+79038347534	Password123	2025-10-24 12:01:16	ACTIVE
\.


--
-- Data for Name: dance_halls; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.dance_halls (id, name, description, capacity, floor_type, equipment, branch_id) FROM stdin;
3d9d31de-9016-41a7-84b4-50ac0dd3a54e	Hall for ballroom dance	Специализированный зал с паркетным покрытием для бальных танцев	20	wooden	зеркала, станок	22222222-2222-2222-2222-222222222222
329629cb-b570-4e94-90de-67190b35815f	Small hall	Уютный зал для индивидуальных занятий и малых групп	10	marley	зеркала, музыкальная система, проектр	22222222-2222-2222-2222-222222222222
78aaef0d-08e3-4ca3-87d7-d6be290843b9	Big spectial hall	Просторный зал для групповых занятий с зеркалами и станками	30	wooden	зеркала, станки, музыкальная система	22222222-2222-2222-2222-222222222222
\.


--
-- Data for Name: enrollments; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.enrollments (id, client_id, lesson_id, status, enrollment_date) FROM stdin;
0449fe2d-03e1-44fe-b96e-d517d44a7728	d76c920d-4cf6-40aa-94e4-21b06292e160	736b7f8d-1fb3-48db-8306-e499d7c85909	REGISTERED	2025-10-16 10:35:11
9d8ee50a-4f70-407f-a228-577fb925f68d	d76c920d-4cf6-40aa-94e4-21b06292e160	087cb1a7-8609-404e-a34a-1f054cb2e292	REGISTERED	2025-10-16 10:35:16
\.


--
-- Data for Name: lessons; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.lessons (id, type, name, description, start_time, duration_minutes, difficulty, max_participants, current_participants, price, status, trainer_id, hall_id) FROM stdin;
b5dd5e00-cdcb-4911-9ad4-ee2207b329bd	OPEN_CLASS	Сальса для всех	Веселое занятие по сальсе для любого уровня подготовки	2025-10-17 15:00:00	60	ALL_LEVELS	14	0	1200.00	SCHEDULED	ff461c7b-f41b-49d0-ae11-0f0464393d72	3d9d31de-9016-41a7-84b4-50ac0dd3a54e
9bea1fcb-a4f8-4777-a688-d16fff8ff49c	MASTERCLASS	Бачата сенсуаль - мастер-класс	Специальный мастер-класс по бачате сенсуаль от чемпиона	2025-10-21 14:00:00	120	ADVANCED	24	0	3000.00	SCHEDULED	ff461c7b-f41b-49d0-ae11-0f0464393d72	78aaef0d-08e3-4ca3-87d7-d6be290843b9
70c83011-31a5-4f19-b0d4-41e54ba17fba	OPEN_CLASS	Хип-хоп для начинающих	Основы хип-хопа, базовые движения и связки	2025-10-20 15:00:00	60	BEGINNER	20	0	1200.00	SCHEDULED	4ef12721-9b81-4b87-a1bc-c85272fff3a6	3d9d31de-9016-41a7-84b4-50ac0dd3a54e
736b7f8d-1fb3-48db-8306-e499d7c85909	OPEN_CLASS	Классический балет для начинающих	Основы классического балета: позиции, основы техники, растяжка	2025-10-18 16:00:00	60	BEGINNER	20	1	1200.00	SCHEDULED	9a8cd4b5-f92f-438c-9289-1dd9d05d3be2	78aaef0d-08e3-4ca3-87d7-d6be290843b9
087cb1a7-8609-404e-a34a-1f054cb2e292	SPECIAL_COURSE	Contemporary - техника и импровизация	Углубленное изучение contemporary dance с элементами импровизации	2025-10-19 12:00:00	180	INTERMEDIATE	10	1	3000.00	SCHEDULED	9a8cd4b5-f92f-438c-9289-1dd9d05d3be2	3d9d31de-9016-41a7-84b4-50ac0dd3a54e
\.


--
-- Data for Name: reviews; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.reviews (id, client_id, lesson_id, rating, comment, publication_date, status) FROM stdin;
\.


--
-- Data for Name: studios; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.studios (id, name, description, contact_email) FROM stdin;
11111111-1111-1111-1111-111111111111	Танцевальная студия "Грация"	Лучшая студия танцев в городе с профессиональными преподавателями	info@gracia-dance.ru
\.


--
-- Data for Name: subscription_types; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.subscription_types (id, name, description, validity_days, visit_count, unlimited, price) FROM stdin;
cccccccc-cccc-cccc-cccc-cccccccccccc	Разовый	Одно посещение любого занятия	1	1	f	500.00
dddddddd-dddd-dddd-dddd-dddddddddddd	Месячный (8 занятий)	8 занятий в течение 30 дней	30	8	f	3000.00
eeeeeeee-eeee-eeee-eeee-eeeeeeeeeeee	Месячный безлимитный	Неограниченное количество занятий в течение 30 дней	30	0	t	5000.00
ffffffff-ffff-ffff-ffff-ffffffffffff	Пробный	3 занятия для новых клиентов	14	3	f	1000.00
\.


--
-- Data for Name: subscriptions; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.subscriptions (id, client_id, subscription_type_id, start_date, end_date, remaining_visits, status, purchase_date) FROM stdin;
390fd435-453c-4c76-b9b9-e5c57e65ea2e	d76c920d-4cf6-40aa-94e4-21b06292e160	eeeeeeee-eeee-eeee-eeee-eeeeeeeeeeee	2025-10-11 11:01:46	2025-11-10 11:01:46	-1	ACTIVE	2025-10-11 11:01:46
124058a8-a879-48af-ac07-3e38dc79822f	f3501785-5446-40c0-800e-9658d0399822	cccccccc-cccc-cccc-cccc-cccccccccccc	2025-10-15 22:01:30	2025-10-16 22:01:30	1	ACTIVE	2025-10-15 22:01:30
\.


--
-- Data for Name: trainer_specializations; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.trainer_specializations (trainer_id, specialization) FROM stdin;
9a8cd4b5-f92f-438c-9289-1dd9d05d3be2	Классический балет
9a8cd4b5-f92f-438c-9289-1dd9d05d3be2	Contemporary
9a8cd4b5-f92f-438c-9289-1dd9d05d3be2	Растяжка
ff461c7b-f41b-49d0-ae11-0f0464393d72	Латина
ff461c7b-f41b-49d0-ae11-0f0464393d72	Сальса
ff461c7b-f41b-49d0-ae11-0f0464393d72	Бачата
ff461c7b-f41b-49d0-ae11-0f0464393d72	Танго
4ef12721-9b81-4b87-a1bc-c85272fff3a6	Хип-хоп
4ef12721-9b81-4b87-a1bc-c85272fff3a6	Джаз-фанк
4ef12721-9b81-4b87-a1bc-c85272fff3a6	Хаус
\.


--
-- Data for Name: trainers; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY public.trainers (id, name, biography, qualification_level, is_active) FROM stdin;
9a8cd4b5-f92f-438c-9289-1dd9d05d3be2	Анна Иванова	Профессиональная балерина с 10-летним опытом преподавания. Специализируется на классическом балете и contemporary.	master	t
ff461c7b-f41b-49d0-ae11-0f0464393d72	Максим Петров	Чемпион России по бальным танцам. Преподает латиноамериканские и европейские танцы.	senior	t
4ef12721-9b81-4b87-a1bc-c85272fff3a6	Елена Смирнова	Специалист по современным танцевальным направлениям: хип-хоп, джаз-фанк, хаус.	middle	t
\.


--
-- Name: bookings bookings_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.bookings
    ADD CONSTRAINT bookings_pkey PRIMARY KEY (id);


--
-- Name: branches branches_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.branches
    ADD CONSTRAINT branches_pkey PRIMARY KEY (id);


--
-- Name: clients clients_email_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.clients
    ADD CONSTRAINT clients_email_key UNIQUE (email);


--
-- Name: clients clients_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.clients
    ADD CONSTRAINT clients_pkey PRIMARY KEY (id);


--
-- Name: dance_halls dance_halls_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.dance_halls
    ADD CONSTRAINT dance_halls_pkey PRIMARY KEY (id);


--
-- Name: enrollments enrollments_client_id_lesson_id_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.enrollments
    ADD CONSTRAINT enrollments_client_id_lesson_id_key UNIQUE (client_id, lesson_id);


--
-- Name: enrollments enrollments_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.enrollments
    ADD CONSTRAINT enrollments_pkey PRIMARY KEY (id);


--
-- Name: lessons lessons_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.lessons
    ADD CONSTRAINT lessons_pkey PRIMARY KEY (id);


--
-- Name: reviews reviews_client_id_lesson_id_key; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.reviews
    ADD CONSTRAINT reviews_client_id_lesson_id_key UNIQUE (client_id, lesson_id);


--
-- Name: reviews reviews_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.reviews
    ADD CONSTRAINT reviews_pkey PRIMARY KEY (id);


--
-- Name: studios studios_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.studios
    ADD CONSTRAINT studios_pkey PRIMARY KEY (id);


--
-- Name: subscription_types subscription_types_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.subscription_types
    ADD CONSTRAINT subscription_types_pkey PRIMARY KEY (id);


--
-- Name: subscriptions subscriptions_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.subscriptions
    ADD CONSTRAINT subscriptions_pkey PRIMARY KEY (id);


--
-- Name: trainer_specializations trainer_specializations_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.trainer_specializations
    ADD CONSTRAINT trainer_specializations_pkey PRIMARY KEY (trainer_id, specialization);


--
-- Name: trainers trainers_pkey; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.trainers
    ADD CONSTRAINT trainers_pkey PRIMARY KEY (id);


--
-- Name: idx_bookings_client_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_bookings_client_id ON public.bookings USING btree (client_id);


--
-- Name: idx_bookings_hall_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_bookings_hall_id ON public.bookings USING btree (hall_id);


--
-- Name: idx_bookings_start_time; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_bookings_start_time ON public.bookings USING btree (start_time);


--
-- Name: idx_clients_email; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_clients_email ON public.clients USING btree (email);


--
-- Name: idx_enrollments_client_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_enrollments_client_id ON public.enrollments USING btree (client_id);


--
-- Name: idx_enrollments_lesson_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_enrollments_lesson_id ON public.enrollments USING btree (lesson_id);


--
-- Name: idx_enrollments_status; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_enrollments_status ON public.enrollments USING btree (status);


--
-- Name: idx_lessons_hall_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_lessons_hall_id ON public.lessons USING btree (hall_id);


--
-- Name: idx_lessons_start_time; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_lessons_start_time ON public.lessons USING btree (start_time);


--
-- Name: idx_lessons_status; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_lessons_status ON public.lessons USING btree (status);


--
-- Name: idx_lessons_trainer_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_lessons_trainer_id ON public.lessons USING btree (trainer_id);


--
-- Name: idx_reviews_rating; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_reviews_rating ON public.reviews USING btree (rating);


--
-- Name: idx_reviews_status; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_reviews_status ON public.reviews USING btree (status);


--
-- Name: idx_subscriptions_client_id; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_subscriptions_client_id ON public.subscriptions USING btree (client_id);


--
-- Name: idx_trainers_active; Type: INDEX; Schema: public; Owner: postgres
--

CREATE INDEX idx_trainers_active ON public.trainers USING btree (is_active);


--
-- Name: bookings bookings_client_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.bookings
    ADD CONSTRAINT bookings_client_id_fkey FOREIGN KEY (client_id) REFERENCES public.clients(id);


--
-- Name: bookings bookings_hall_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.bookings
    ADD CONSTRAINT bookings_hall_id_fkey FOREIGN KEY (hall_id) REFERENCES public.dance_halls(id);


--
-- Name: branches branches_studio_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.branches
    ADD CONSTRAINT branches_studio_id_fkey FOREIGN KEY (studio_id) REFERENCES public.studios(id);


--
-- Name: dance_halls dance_halls_branch_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.dance_halls
    ADD CONSTRAINT dance_halls_branch_id_fkey FOREIGN KEY (branch_id) REFERENCES public.branches(id);


--
-- Name: enrollments enrollments_client_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.enrollments
    ADD CONSTRAINT enrollments_client_id_fkey FOREIGN KEY (client_id) REFERENCES public.clients(id);


--
-- Name: enrollments enrollments_lesson_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.enrollments
    ADD CONSTRAINT enrollments_lesson_id_fkey FOREIGN KEY (lesson_id) REFERENCES public.lessons(id);


--
-- Name: lessons lessons_hall_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.lessons
    ADD CONSTRAINT lessons_hall_id_fkey FOREIGN KEY (hall_id) REFERENCES public.dance_halls(id);


--
-- Name: lessons lessons_trainer_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.lessons
    ADD CONSTRAINT lessons_trainer_id_fkey FOREIGN KEY (trainer_id) REFERENCES public.trainers(id);


--
-- Name: reviews reviews_client_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.reviews
    ADD CONSTRAINT reviews_client_id_fkey FOREIGN KEY (client_id) REFERENCES public.clients(id);


--
-- Name: reviews reviews_lesson_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.reviews
    ADD CONSTRAINT reviews_lesson_id_fkey FOREIGN KEY (lesson_id) REFERENCES public.lessons(id);


--
-- Name: subscriptions subscriptions_client_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.subscriptions
    ADD CONSTRAINT subscriptions_client_id_fkey FOREIGN KEY (client_id) REFERENCES public.clients(id);


--
-- Name: subscriptions subscriptions_subscription_type_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.subscriptions
    ADD CONSTRAINT subscriptions_subscription_type_id_fkey FOREIGN KEY (subscription_type_id) REFERENCES public.subscription_types(id);


--
-- Name: trainer_specializations trainer_specializations_trainer_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.trainer_specializations
    ADD CONSTRAINT trainer_specializations_trainer_id_fkey FOREIGN KEY (trainer_id) REFERENCES public.trainers(id);


--
-- Name: TABLE bookings; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.bookings TO dance_user;


--
-- Name: TABLE branches; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.branches TO dance_user;


--
-- Name: TABLE clients; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.clients TO dance_user;


--
-- Name: TABLE dance_halls; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.dance_halls TO dance_user;


--
-- Name: TABLE enrollments; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.enrollments TO dance_user;


--
-- Name: TABLE lessons; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.lessons TO dance_user;


--
-- Name: TABLE reviews; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.reviews TO dance_user;


--
-- Name: TABLE studios; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.studios TO dance_user;


--
-- Name: TABLE subscription_types; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.subscription_types TO dance_user;


--
-- Name: TABLE subscriptions; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.subscriptions TO dance_user;


--
-- Name: TABLE trainer_specializations; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.trainer_specializations TO dance_user;


--
-- Name: TABLE trainers; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.trainers TO dance_user;


--
-- PostgreSQL database dump complete
--

\unrestrict sjkgdCnWWNPSIChp9Mfko0ahF3iLEkOea56RsyuHqVjQIBxQHDfJRTHKwR1Yz4g

