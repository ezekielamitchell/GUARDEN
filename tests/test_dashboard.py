"""
Tests for GUARDEN Flask dashboard API endpoints.
Run: pytest tests/test_dashboard.py -v
"""

import pytest
from unittest.mock import MagicMock, patch


@pytest.fixture
def client():
    """Create Flask test client with mocked DB."""
    with patch("fog.dashboard.app.get_db") as mock_db:
        mock_conn = MagicMock()
        mock_cursor = MagicMock()
        mock_conn.__enter__ = MagicMock(return_value=mock_conn)
        mock_conn.__exit__ = MagicMock(return_value=False)
        mock_conn.cursor.return_value.__enter__ = MagicMock(return_value=mock_cursor)
        mock_conn.cursor.return_value.__exit__ = MagicMock(return_value=False)
        mock_db.return_value = mock_conn

        from fog.dashboard.app import app
        app.config["TESTING"] = True
        with app.test_client() as c:
            yield c, mock_cursor


class TestDashboardRoutes:
    def test_index_returns_200(self, client):
        c, _ = client
        resp = c.get("/")
        assert resp.status_code == 200

    def test_heatmap_returns_json(self, client):
        c, mock_cursor = client
        mock_cursor.fetchall.return_value = [
            {"lat": 47.598, "lng": -122.318, "confidence": 0.87}
        ]
        resp = c.get("/api/heatmap")
        assert resp.status_code == 200
        data = resp.get_json()
        assert isinstance(data, list)
        assert len(data) == 1
        assert data[0][0] == 47.598

    def test_timeline_returns_json(self, client):
        c, mock_cursor = client
        mock_cursor.fetchall.return_value = []
        resp = c.get("/api/timeline")
        assert resp.status_code == 200
        assert isinstance(resp.get_json(), list)

    def test_nodes_returns_json(self, client):
        c, mock_cursor = client
        mock_cursor.fetchall.return_value = []
        resp = c.get("/api/nodes")
        assert resp.status_code == 200
        assert isinstance(resp.get_json(), list)

    def test_recent_returns_json(self, client):
        c, mock_cursor = client
        mock_cursor.fetchall.return_value = []
        resp = c.get("/api/detections/recent")
        assert resp.status_code == 200
        assert isinstance(resp.get_json(), list)

    def test_stats_returns_json(self, client):
        c, mock_cursor = client
        from datetime import datetime, timezone
        mock_cursor.fetchone.return_value = {
            "total_detections": 42,
            "active_nodes": 3,
            "last_detection": datetime.now(timezone.utc),
            "avg_confidence": 0.78,
            "detections_24h": 12,
        }
        resp = c.get("/api/stats")
        assert resp.status_code == 200
        data = resp.get_json()
        assert data["total_detections"] == 42
        assert data["active_nodes"] == 3
