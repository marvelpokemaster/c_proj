CREATE TABLE vehicles (
    vehicle_id SERIAL PRIMARY KEY,
    name VARCHAR(100),
    price NUMERIC,
    type VARCHAR(50),
    height NUMERIC,
    width NUMERIC,
    color VARCHAR(50)
);

CREATE TABLE customers (
    customer_id SERIAL PRIMARY KEY,
    name VARCHAR(100),
    address TEXT,
    phone VARCHAR(15)
);

CREATE TABLE sales (
    sale_id SERIAL PRIMARY KEY,
    vehicle_id INT REFERENCES vehicles(vehicle_id),
    customer_id INT REFERENCES customers(customer_id),
    amount_paid NUMERIC,
    full_payment BOOLEAN,
    sale_date TIMESTAMP DEFAULT NOW()
);
