DROP SCHEMA IF EXISTS yl_schema CASCADE;

CREATE SCHEMA IF NOT EXISTS yl_schema;


CREATE TABLE IF NOT EXISTS yl_schema.orders (
    order_id BIGSERIAL PRIMARY KEY,
    weight INTEGER DEFAULT(1) NOT NULL,
    regions INTEGER DEFAULT(1) NOT NULL,
    delivery_hours TEXT[] NOT NULL,
    cost INTEGER DEFAULT(1) NOT NULL,
    completed_time TIMESTAMP
);

CREATE TABLE IF NOT EXISTS yl_schema.couriers (
    courier_id BIGSERIAL PRIMARY KEY,
    courier_type TEXT,
    regions INTEGER[],
    working_hours TEXT[]
);

CREATE TABLE IF NOT EXISTS yl_schema.completed_orders (
    courier_id INTEGER NOT NULL,
    order_id INTEGER NOT NULL
);