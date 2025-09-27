"""SQLAlchemy models for the Ladder service."""
from __future__ import annotations

from datetime import datetime

from sqlalchemy import Column, DateTime, Integer, String, Text
from sqlalchemy.orm import declarative_base


Base = declarative_base()


class Match(Base):
    """Represents a single match reported by a Q3Rally server."""

    __tablename__ = "matches"

    id = Column(Integer, primary_key=True, autoincrement=True)
    match_id = Column(String(64), unique=True, nullable=False, index=True)
    mode = Column(String(32), nullable=False, index=True)
    start_time = Column(DateTime(timezone=True), nullable=False)
    end_time = Column(DateTime(timezone=True), nullable=False)
    payload = Column(Text, nullable=False)
    created_at = Column(DateTime(timezone=True), default=datetime.utcnow, nullable=False)
