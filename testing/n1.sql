CREATE DATABASE dealership;

\c dealership;

CREATE TABLE customers (
    id SERIAL PRIMARY KEY,
    name VARCHAR(50),
    phone VARCHAR(15),
    address VARCHAR(100),
    email VARCHAR(50),
    car_model VARCHAR(50),
    budget NUMERIC(10, 2)
);
