"""
GUARDEN — MQTT Ingestion Service
Subscribes to all node detection topics and writes to PostgreSQL.
Run: python3 fog/ingestion/main.py
"""

import asyncio
import json
import logging
import os
from datetime import datetime, timezone

import aiomqtt
import asyncpg
from dotenv import load_dotenv

load_dotenv()

# ─── Config ───────────────────────────────────────────────────────────────────
MQTT_BROKER   = os.getenv("MQTT_BROKER", "localhost")
MQTT_PORT     = int(os.getenv("MQTT_PORT", 1883))
DATABASE_URL  = os.getenv("DATABASE_URL", "postgresql://guarden:guarden@localhost/guarden")
SUBSCRIBE_TOPIC = "guarden/nodes/+/detection"

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s"
)
log = logging.getLogger(__name__)


# ─── DB ───────────────────────────────────────────────────────────────────────
async def insert_detection(pool: asyncpg.Pool, payload: dict) -> None:
    async with pool.acquire() as conn:
        await conn.execute(
            """
            INSERT INTO detections (node_id, timestamp, confidence, lat, lng, battery_mv, temp_c)
            VALUES ($1, $2, $3, $4, $5, $6, $7)
            """,
            payload["node_id"],
            datetime.fromtimestamp(payload.get("timestamp", 0), tz=timezone.utc),
            float(payload["confidence"]),
            float(payload["location"]["lat"]),
            float(payload["location"]["lng"]),
            payload.get("battery_mv"),
            payload.get("temp_c"),
        )
    log.info(f"Stored detection | node={payload['node_id']} conf={payload['confidence']:.2f}")


# ─── Main Loop ────────────────────────────────────────────────────────────────
async def main() -> None:
    log.info(f"Connecting to DB: {DATABASE_URL}")
    pool = await asyncpg.create_pool(DATABASE_URL)

    log.info(f"Connecting to MQTT: {MQTT_BROKER}:{MQTT_PORT}")
    async with aiomqtt.Client(MQTT_BROKER, MQTT_PORT) as client:
        await client.subscribe(SUBSCRIBE_TOPIC)
        log.info(f"Subscribed to: {SUBSCRIBE_TOPIC}")

        async for message in client.messages:
            try:
                payload = json.loads(message.payload.decode())
                await insert_detection(pool, payload)
            except (json.JSONDecodeError, KeyError) as e:
                log.warning(f"Bad payload from {message.topic}: {e}")
            except Exception as e:
                log.error(f"Ingestion error: {e}")


if __name__ == "__main__":
    asyncio.run(main())
