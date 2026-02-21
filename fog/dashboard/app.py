"""
GUARDEN — Flask Dashboard
Local web dashboard for rat detection visualization.
Run: python3 fog/dashboard/app.py
"""

import os
from datetime import datetime, timedelta, timezone

import psycopg2
import psycopg2.extras
from dotenv import load_dotenv
from flask import Flask, jsonify, render_template, request

load_dotenv()

app = Flask(__name__)
DATABASE_URL = os.getenv("DATABASE_URL", "postgresql://guarden:guarden@localhost/guarden")


# ─── DB Helper ────────────────────────────────────────────────────────────────
def get_db():
    return psycopg2.connect(DATABASE_URL, cursor_factory=psycopg2.extras.RealDictCursor)


# ─── Pages ────────────────────────────────────────────────────────────────────
@app.route("/")
def index():
    return render_template("index.html")


# ─── API: Heatmap data ────────────────────────────────────────────────────────
@app.route("/api/heatmap")
def api_heatmap():
    """Returns detection points for Leaflet.heat layer."""
    hours = int(request.args.get("hours", 24))
    since = datetime.now(timezone.utc) - timedelta(hours=hours)

    with get_db() as conn, conn.cursor() as cur:
        cur.execute(
            """
            SELECT lat, lng, confidence
            FROM detections
            WHERE timestamp >= %s
            ORDER BY timestamp DESC
            """,
            (since,),
        )
        rows = cur.fetchall()

    # Leaflet.heat expects [[lat, lng, intensity], ...]
    points = [[r["lat"], r["lng"], r["confidence"]] for r in rows]
    return jsonify(points)


# ─── API: Activity timeline ───────────────────────────────────────────────────
@app.route("/api/timeline")
def api_timeline():
    """Returns hourly detection counts for Chart.js."""
    hours = int(request.args.get("hours", 48))
    since = datetime.now(timezone.utc) - timedelta(hours=hours)

    with get_db() as conn, conn.cursor() as cur:
        cur.execute(
            """
            SELECT
                date_trunc('hour', timestamp) AS hour,
                COUNT(*) AS count
            FROM detections
            WHERE timestamp >= %s
            GROUP BY hour
            ORDER BY hour ASC
            """,
            (since,),
        )
        rows = cur.fetchall()

    data = [{"hour": r["hour"].isoformat(), "count": r["count"]} for r in rows]
    return jsonify(data)


# ─── API: Node status ─────────────────────────────────────────────────────────
@app.route("/api/nodes")
def api_nodes():
    """Returns all nodes with last detection time and battery."""
    with get_db() as conn, conn.cursor() as cur:
        cur.execute(
            """
            SELECT
                n.node_id,
                n.location_label,
                n.lat,
                n.lng,
                n.active,
                MAX(d.timestamp)   AS last_seen,
                MAX(d.battery_mv)  AS battery_mv,
                COUNT(d.id)        AS total_detections
            FROM nodes n
            LEFT JOIN detections d ON n.node_id = d.node_id
            GROUP BY n.node_id, n.location_label, n.lat, n.lng, n.active
            ORDER BY n.node_id
            """
        )
        rows = cur.fetchall()

    nodes = []
    for r in rows:
        nodes.append({
            "node_id":           r["node_id"],
            "location_label":    r["location_label"],
            "lat":               r["lat"],
            "lng":               r["lng"],
            "active":            r["active"],
            "last_seen":         r["last_seen"].isoformat() if r["last_seen"] else None,
            "battery_mv":        r["battery_mv"],
            "total_detections":  r["total_detections"],
        })
    return jsonify(nodes)


# ─── API: Recent detections ───────────────────────────────────────────────────
@app.route("/api/detections/recent")
def api_recent():
    """Returns the 50 most recent detections."""
    with get_db() as conn, conn.cursor() as cur:
        cur.execute(
            """
            SELECT node_id, timestamp, confidence, lat, lng, battery_mv
            FROM detections
            ORDER BY timestamp DESC
            LIMIT 50
            """
        )
        rows = cur.fetchall()

    detections = [
        {
            "node_id":    r["node_id"],
            "timestamp":  r["timestamp"].isoformat(),
            "confidence": round(r["confidence"], 2),
            "lat":        r["lat"],
            "lng":        r["lng"],
            "battery_mv": r["battery_mv"],
        }
        for r in rows
    ]
    return jsonify(detections)


# ─── API: Stats summary ───────────────────────────────────────────────────────
@app.route("/api/stats")
def api_stats():
    """Returns summary stats for the dashboard header."""
    with get_db() as conn, conn.cursor() as cur:
        cur.execute(
            """
            SELECT
                COUNT(*)                                        AS total_detections,
                COUNT(DISTINCT node_id)                        AS active_nodes,
                MAX(timestamp)                                 AS last_detection,
                AVG(confidence)                                AS avg_confidence,
                COUNT(*) FILTER (WHERE timestamp >= NOW() - INTERVAL '24h') AS detections_24h
            FROM detections
            """
        )
        row = cur.fetchone()

    return jsonify({
        "total_detections": row["total_detections"],
        "active_nodes":     row["active_nodes"],
        "last_detection":   row["last_detection"].isoformat() if row["last_detection"] else None,
        "avg_confidence":   round(row["avg_confidence"] or 0, 2),
        "detections_24h":   row["detections_24h"],
    })


if __name__ == "__main__":
    # Mobile-accessible on local network
    app.run(host="0.0.0.0", port=5000, debug=False)
