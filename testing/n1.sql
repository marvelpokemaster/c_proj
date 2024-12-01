CREATE TABLE vehicles (
    vehicle_id SERIAL PRIMARY KEY,
    name VARCHAR(100),
    details TEXT,
    price NUMERIC
);

CREATE TABLE customers (    phone VARCHAR(15)
);

CREATE TABLE sales (
    sale_id SERIAL PRIMARY KEY,
    vehicle_id INT REFERENCES vehicles(vehicle_id),
    customer_id INT REFERENCES customers(customer_id),
    amount_paid NUMERIC,
    full_payment BOOLEAN,
    sale_date TIMESTAMP DEFAULT NOW()
);
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
