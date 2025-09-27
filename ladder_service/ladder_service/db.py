"""Database setup for the Ladder service."""
from __future__ import annotations

from contextlib import contextmanager
from pathlib import Path
from typing import Iterator

from sqlalchemy import Engine, create_engine
from sqlalchemy.orm import Session, sessionmaker


def _database_url(db_path: str | Path | None) -> str:
    if db_path is None:
        return "sqlite:///./ladder.db"
    path = Path(db_path)
    if path.is_dir():
        path = path / "ladder.db"
    return f"sqlite:///{path}"


def create_session_factory(
    db_path: str | Path | None = None,
) -> tuple[Engine, sessionmaker[Session]]:
    engine = create_engine(
        _database_url(db_path), connect_args={"check_same_thread": False}
    )
    factory = sessionmaker(autocommit=False, autoflush=False, bind=engine)
    return engine, factory


@contextmanager
def session_scope(session_factory: sessionmaker[Session]) -> Iterator[Session]:
    session = session_factory()
    try:
        yield session
        session.commit()
    except Exception:
        session.rollback()
        raise
    finally:
        session.close()
