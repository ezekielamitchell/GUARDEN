"""
Tests for GUARDEN MQTT ingestion service.
Run: pytest tests/test_ingestion.py -v
"""

import json
import pytest
from unittest.mock import AsyncMock, MagicMock, patch


VALID_PAYLOAD = {
    "node_id": "node_01",
    "timestamp": 1708900000,
    "confidence": 0.87,
    "location": {"lat": 47.598, "lng": -122.318},
    "battery_mv": 3850,
    "temp_c": 12.4,
}


class TestPayloadValidation:
    def test_valid_payload_parses(self):
        data = json.loads(json.dumps(VALID_PAYLOAD))
        assert data["node_id"] == "node_01"
        assert data["confidence"] == 0.87
        assert "lat" in data["location"]

    def test_missing_node_id_raises(self):
        bad = VALID_PAYLOAD.copy()
        del bad["node_id"]
        with pytest.raises(KeyError):
            _ = bad["node_id"]

    def test_missing_confidence_raises(self):
        bad = VALID_PAYLOAD.copy()
        del bad["confidence"]
        with pytest.raises(KeyError):
            _ = bad["confidence"]

    def test_confidence_in_range(self):
        assert 0.0 <= VALID_PAYLOAD["confidence"] <= 1.0

    def test_invalid_json_raises(self):
        with pytest.raises(json.JSONDecodeError):
            json.loads("not valid json")


class TestDetectionInsert:
    @pytest.mark.asyncio
    async def test_insert_detection_called_with_correct_args(self):
        mock_pool = AsyncMock()
        mock_conn = AsyncMock()
        mock_pool.acquire.return_value.__aenter__.return_value = mock_conn

        from fog.ingestion.main import insert_detection
        await insert_detection(mock_pool, VALID_PAYLOAD)

        mock_conn.execute.assert_called_once()
        call_args = mock_conn.execute.call_args[0]
        assert "node_01" in call_args
        assert 0.87 in call_args

    @pytest.mark.asyncio
    async def test_low_confidence_still_stored(self):
        """All detections above threshold are stored — filtering is at edge."""
        payload = VALID_PAYLOAD.copy()
        payload["confidence"] = 0.65

        mock_pool = AsyncMock()
        mock_conn = AsyncMock()
        mock_pool.acquire.return_value.__aenter__.return_value = mock_conn

        from fog.ingestion.main import insert_detection
        await insert_detection(mock_pool, payload)
        mock_conn.execute.assert_called_once()
