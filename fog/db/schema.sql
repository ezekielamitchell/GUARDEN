-- GUARDEN PostgreSQL Schema
-- Run once: psql -U guarden -d guarden -f schema.sql

-- Detection events from ESP32 nodes
CREATE TABLE IF NOT EXISTS detections (
    id          SERIAL PRIMARY KEY,
    node_id     VARCHAR(20)   NOT NULL,
    timestamp   TIMESTAMPTZ   NOT NULL DEFAULT NOW(),
    confidence  FLOAT         NOT NULL,
    lat         FLOAT         NOT NULL,
    lng         FLOAT         NOT NULL,
    battery_mv  INT,
    temp_c      FLOAT
);

-- Node registry
CREATE TABLE IF NOT EXISTS nodes (
    node_id        VARCHAR(20)  PRIMARY KEY,
    location_label VARCHAR(100),
    lat            FLOAT        NOT NULL,
    lng            FLOAT        NOT NULL,
    installed_at   TIMESTAMPTZ  DEFAULT NOW(),
    active         BOOLEAN      DEFAULT TRUE
);

-- Indexes for common dashboard queries
CREATE INDEX IF NOT EXISTS idx_detections_node_id   ON detections(node_id);
CREATE INDEX IF NOT EXISTS idx_detections_timestamp ON detections(timestamp DESC);

-- Seed node registry (update coords to match actual garden layout)
INSERT INTO nodes (node_id, location_label, lat, lng) VALUES
    ('node_01', 'North Beds',     47.5982, -122.3181),
    ('node_02', 'South Beds',     47.5978, -122.3180),
    ('node_03', 'Compost Area',   47.5979, -122.3183),
    ('node_04', 'Tool Shed',      47.5981, -122.3178)
ON CONFLICT (node_id) DO NOTHING;
