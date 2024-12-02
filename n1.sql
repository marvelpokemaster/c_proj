CREATE TABLE vehicles (
    vehicle_id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    type VARCHAR(50) NOT NULL,
    color VARCHAR(50) NOT NULL,
    price NUMERIC(12, 2) NOT NULL,  -- Allows up to 9999999999.99
    height NUMERIC(10, 2),         -- Allows up to 99999999.99
    width NUMERIC(10, 2)           -- Allows up to 99999999.99
);

CREATE TABLE customers (
    customer_id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    address TEXT NOT NULL,
    phone VARCHAR(15) NOT NULL
);

CREATE TABLE sales (
    sale_id SERIAL PRIMARY KEY,
    vehicle_id INT REFERENCES vehicles(vehicle_id) ON DELETE CASCADE,
    customer_id INT REFERENCES customers(customer_id) ON DELETE CASCADE,
    amount_paid NUMERIC(10, 2) NOT NULL,
    full_payment BOOLEAN DEFAULT FALSE
);
