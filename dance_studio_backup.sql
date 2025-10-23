--
-- PostgreSQL database dump
--

\restrict Mqo5tKc9efgaooe5xbao4jaKVjggB6ZKMw3IT8pZMzodHhBnqDnaMnuM8EC6eFB

-- Dumped from database version 15.14 (Debian 15.14-1.pgdg13+1)
-- Dumped by pg_dump version 15.14 (Debian 15.14-1.pgdg13+1)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
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
-- Name: bookings; Type: TABLE; Schema: public; Owner: dance_user
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


ALTER TABLE public.bookings OWNER TO dance_user;

--
-- Name: branches; Type: TABLE; Schema: public; Owner: dance_user
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


ALTER TABLE public.branches OWNER TO dance_user;

--
-- Name: clients; Type: TABLE; Schema: public; Owner: dance_user
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


ALTER TABLE public.clients OWNER TO dance_user;

--
-- Name: dance_halls; Type: TABLE; Schema: public; Owner: dance_user
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


ALTER TABLE public.dance_halls OWNER TO dance_user;

--
-- Name: enrollments; Type: TABLE; Schema: public; Owner: dance_user
--

CREATE TABLE public.enrollments (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    client_id uuid NOT NULL,
    lesson_id uuid NOT NULL,
    status character varying(20) DEFAULT 'REGISTERED'::character varying NOT NULL,
    enrollment_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    CONSTRAINT enrollments_status_check CHECK (((status)::text = ANY ((ARRAY['REGISTERED'::character varying, 'CANCELLED'::character varying, 'ATTENDED'::character varying, 'MISSED'::character varying])::text[])))
);


ALTER TABLE public.enrollments OWNER TO dance_user;

--
-- Name: lessons; Type: TABLE; Schema: public; Owner: dance_user
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


ALTER TABLE public.lessons OWNER TO dance_user;

--
-- Name: reviews; Type: TABLE; Schema: public; Owner: dance_user
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


ALTER TABLE public.reviews OWNER TO dance_user;

--
-- Name: studios; Type: TABLE; Schema: public; Owner: dance_user
--

CREATE TABLE public.studios (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    name character varying(255) NOT NULL,
    description text,
    contact_email character varying(255) NOT NULL
);


ALTER TABLE public.studios OWNER TO dance_user;

--
-- Name: subscription_types; Type: TABLE; Schema: public; Owner: dance_user
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


ALTER TABLE public.subscription_types OWNER TO dance_user;

--
-- Name: subscriptions; Type: TABLE; Schema: public; Owner: dance_user
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


ALTER TABLE public.subscriptions OWNER TO dance_user;

--
-- Name: trainer_specializations; Type: TABLE; Schema: public; Owner: dance_user
--

CREATE TABLE public.trainer_specializations (
    trainer_id uuid NOT NULL,
    specialization character varying(100) NOT NULL
);


ALTER TABLE public.trainer_specializations OWNER TO dance_user;

--
-- Name: trainers; Type: TABLE; Schema: public; Owner: dance_user
--

CREATE TABLE public.trainers (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    name character varying(255) NOT NULL,
    biography text,
    qualification_level character varying(50) NOT NULL,
    is_active boolean DEFAULT true NOT NULL
);


ALTER TABLE public.trainers OWNER TO dance_user;

--
-- Data for Name: bookings; Type: TABLE DATA; Schema: public; Owner: dance_user
--

COPY public.bookings (id, client_id, hall_id, start_time, duration_minutes, purpose, status, created_at) FROM stdin;
\.


--
-- Data for Name: branches; Type: TABLE DATA; Schema: public; Owner: dance_user
--

COPY public.branches (id, name, address, phone, open_time, close_time, studio_id) FROM stdin;
\.


--
-- Data for Name: clients; Type: TABLE DATA; Schema: public; Owner: dance_user
--

COPY public.clients (id, name, email, phone, password_hash, registration_date, status) FROM stdin;
a67ecd4e-7032-4df2-a305-91ef461bda19	Shchukin Nik	shchukin-nik5110@yandex.ru	+79250386085	Password123	2025-10-18 09:18:00	ACTIVE
\.


--
-- Data for Name: dance_halls; Type: TABLE DATA; Schema: public; Owner: dance_user
--

COPY public.dance_halls (id, name, description, capacity, floor_type, equipment, branch_id) FROM stdin;
\.


--
-- Data for Name: enrollments; Type: TABLE DATA; Schema: public; Owner: dance_user
--

COPY public.enrollments (id, client_id, lesson_id, status, enrollment_date) FROM stdin;
\.


--
-- Data for Name: lessons; Type: TABLE DATA; Schema: public; Owner: dance_user
--

COPY public.lessons (id, type, name, description, start_time, duration_minutes, difficulty, max_participants, current_participants, price, status, trainer_id, hall_id) FROM stdin;
\.


--
-- Data for Name: reviews; Type: TABLE DATA; Schema: public; Owner: dance_user
--

COPY public.reviews (id, client_id, lesson_id, rating, comment, publication_date, status) FROM stdin;
\.


--
-- Data for Name: studios; Type: TABLE DATA; Schema: public; Owner: dance_user
--

COPY public.studios (id, name, description, contact_email) FROM stdin;
\.


--
-- Data for Name: subscription_types; Type: TABLE DATA; Schema: public; Owner: dance_user
--

COPY public.subscription_types (id, name, description, validity_days, visit_count, unlimited, price) FROM stdin;
\.


--
-- Data for Name: subscriptions; Type: TABLE DATA; Schema: public; Owner: dance_user
--

COPY public.subscriptions (id, client_id, subscription_type_id, start_date, end_date, remaining_visits, status, purchase_date) FROM stdin;
\.


--
-- Data for Name: trainer_specializations; Type: TABLE DATA; Schema: public; Owner: dance_user
--

COPY public.trainer_specializations (trainer_id, specialization) FROM stdin;
\.


--
-- Data for Name: trainers; Type: TABLE DATA; Schema: public; Owner: dance_user
--

COPY public.trainers (id, name, biography, qualification_level, is_active) FROM stdin;
\.


--
-- Name: bookings bookings_pkey; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.bookings
    ADD CONSTRAINT bookings_pkey PRIMARY KEY (id);


--
-- Name: branches branches_pkey; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.branches
    ADD CONSTRAINT branches_pkey PRIMARY KEY (id);


--
-- Name: clients clients_email_key; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.clients
    ADD CONSTRAINT clients_email_key UNIQUE (email);


--
-- Name: clients clients_pkey; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.clients
    ADD CONSTRAINT clients_pkey PRIMARY KEY (id);


--
-- Name: dance_halls dance_halls_pkey; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.dance_halls
    ADD CONSTRAINT dance_halls_pkey PRIMARY KEY (id);


--
-- Name: enrollments enrollments_client_id_lesson_id_key; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.enrollments
    ADD CONSTRAINT enrollments_client_id_lesson_id_key UNIQUE (client_id, lesson_id);


--
-- Name: enrollments enrollments_pkey; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.enrollments
    ADD CONSTRAINT enrollments_pkey PRIMARY KEY (id);


--
-- Name: lessons lessons_pkey; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.lessons
    ADD CONSTRAINT lessons_pkey PRIMARY KEY (id);


--
-- Name: reviews reviews_client_id_lesson_id_key; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.reviews
    ADD CONSTRAINT reviews_client_id_lesson_id_key UNIQUE (client_id, lesson_id);


--
-- Name: reviews reviews_pkey; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.reviews
    ADD CONSTRAINT reviews_pkey PRIMARY KEY (id);


--
-- Name: studios studios_pkey; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.studios
    ADD CONSTRAINT studios_pkey PRIMARY KEY (id);


--
-- Name: subscription_types subscription_types_pkey; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.subscription_types
    ADD CONSTRAINT subscription_types_pkey PRIMARY KEY (id);


--
-- Name: subscriptions subscriptions_pkey; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.subscriptions
    ADD CONSTRAINT subscriptions_pkey PRIMARY KEY (id);


--
-- Name: trainer_specializations trainer_specializations_pkey; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.trainer_specializations
    ADD CONSTRAINT trainer_specializations_pkey PRIMARY KEY (trainer_id, specialization);


--
-- Name: trainers trainers_pkey; Type: CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.trainers
    ADD CONSTRAINT trainers_pkey PRIMARY KEY (id);


--
-- Name: idx_bookings_client_id; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_bookings_client_id ON public.bookings USING btree (client_id);


--
-- Name: idx_bookings_hall_id; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_bookings_hall_id ON public.bookings USING btree (hall_id);


--
-- Name: idx_bookings_start_time; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_bookings_start_time ON public.bookings USING btree (start_time);


--
-- Name: idx_clients_email; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_clients_email ON public.clients USING btree (email);


--
-- Name: idx_enrollments_client_id; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_enrollments_client_id ON public.enrollments USING btree (client_id);


--
-- Name: idx_enrollments_lesson_id; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_enrollments_lesson_id ON public.enrollments USING btree (lesson_id);


--
-- Name: idx_enrollments_status; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_enrollments_status ON public.enrollments USING btree (status);


--
-- Name: idx_lessons_hall_id; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_lessons_hall_id ON public.lessons USING btree (hall_id);


--
-- Name: idx_lessons_start_time; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_lessons_start_time ON public.lessons USING btree (start_time);


--
-- Name: idx_lessons_status; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_lessons_status ON public.lessons USING btree (status);


--
-- Name: idx_lessons_trainer_id; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_lessons_trainer_id ON public.lessons USING btree (trainer_id);


--
-- Name: idx_reviews_rating; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_reviews_rating ON public.reviews USING btree (rating);


--
-- Name: idx_reviews_status; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_reviews_status ON public.reviews USING btree (status);


--
-- Name: idx_subscriptions_client_id; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_subscriptions_client_id ON public.subscriptions USING btree (client_id);


--
-- Name: idx_trainers_active; Type: INDEX; Schema: public; Owner: dance_user
--

CREATE INDEX idx_trainers_active ON public.trainers USING btree (is_active);


--
-- Name: bookings bookings_client_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.bookings
    ADD CONSTRAINT bookings_client_id_fkey FOREIGN KEY (client_id) REFERENCES public.clients(id);


--
-- Name: bookings bookings_hall_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.bookings
    ADD CONSTRAINT bookings_hall_id_fkey FOREIGN KEY (hall_id) REFERENCES public.dance_halls(id);


--
-- Name: branches branches_studio_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.branches
    ADD CONSTRAINT branches_studio_id_fkey FOREIGN KEY (studio_id) REFERENCES public.studios(id);


--
-- Name: dance_halls dance_halls_branch_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.dance_halls
    ADD CONSTRAINT dance_halls_branch_id_fkey FOREIGN KEY (branch_id) REFERENCES public.branches(id);


--
-- Name: enrollments enrollments_client_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.enrollments
    ADD CONSTRAINT enrollments_client_id_fkey FOREIGN KEY (client_id) REFERENCES public.clients(id);


--
-- Name: enrollments enrollments_lesson_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.enrollments
    ADD CONSTRAINT enrollments_lesson_id_fkey FOREIGN KEY (lesson_id) REFERENCES public.lessons(id);


--
-- Name: lessons lessons_hall_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.lessons
    ADD CONSTRAINT lessons_hall_id_fkey FOREIGN KEY (hall_id) REFERENCES public.dance_halls(id);


--
-- Name: lessons lessons_trainer_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.lessons
    ADD CONSTRAINT lessons_trainer_id_fkey FOREIGN KEY (trainer_id) REFERENCES public.trainers(id);


--
-- Name: reviews reviews_client_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.reviews
    ADD CONSTRAINT reviews_client_id_fkey FOREIGN KEY (client_id) REFERENCES public.clients(id);


--
-- Name: reviews reviews_lesson_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.reviews
    ADD CONSTRAINT reviews_lesson_id_fkey FOREIGN KEY (lesson_id) REFERENCES public.lessons(id);


--
-- Name: subscriptions subscriptions_client_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.subscriptions
    ADD CONSTRAINT subscriptions_client_id_fkey FOREIGN KEY (client_id) REFERENCES public.clients(id);


--
-- Name: subscriptions subscriptions_subscription_type_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.subscriptions
    ADD CONSTRAINT subscriptions_subscription_type_id_fkey FOREIGN KEY (subscription_type_id) REFERENCES public.subscription_types(id);


--
-- Name: trainer_specializations trainer_specializations_trainer_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dance_user
--

ALTER TABLE ONLY public.trainer_specializations
    ADD CONSTRAINT trainer_specializations_trainer_id_fkey FOREIGN KEY (trainer_id) REFERENCES public.trainers(id);


--
-- PostgreSQL database dump complete
--

\unrestrict Mqo5tKc9efgaooe5xbao4jaKVjggB6ZKMw3IT8pZMzodHhBnqDnaMnuM8EC6eFB

