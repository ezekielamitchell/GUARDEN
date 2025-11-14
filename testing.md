# Testing

```bash
# Run all tests
pytest tests/

# Test by tier
pytest tests/edge/      # Edge tier
pytest tests/fog/       # Fog tier
pytest tests/web/       # Web tier
pytest tests/integration/  # Integration tests

# Test coverage
pytest --cov=src tests/
```
