"""FastAPI application exposing the Ladder webservice."""
from __future__ import annotations

import json
import os
from contextlib import asynccontextmanager
from typing import AsyncIterator

from fastapi import Depends, FastAPI, HTTPException, Path, Query, status
from fastapi.responses import JSONResponse
from sqlalchemy import Engine, select
from sqlalchemy.exc import IntegrityError
from sqlalchemy.orm import Session, sessionmaker

from .db import create_session_factory, session_scope
from .models import Base, Match
from .schemas import MatchCreate


def _init_session_factory() -> tuple[sessionmaker[Session], Engine]:
    db_path = os.getenv("LADDER_DB_PATH")
    engine, factory = create_session_factory(db_path)
    Base.metadata.create_all(bind=engine)
    return factory, engine


SessionFactory, Engine = _init_session_factory()


def get_session() -> Session:
    """Provide a scoped SQLAlchemy session for FastAPI dependencies."""

    with session_scope(SessionFactory) as session:
        yield session


@asynccontextmanager
async def lifespan(_: FastAPI) -> AsyncIterator[None]:
    Base.metadata.create_all(bind=Engine)
    yield


app = FastAPI(
    title="Q3Rally Ladder Service",
    version="0.1.0",
    lifespan=lifespan,
)


@app.post(
    "/api/v1/matches",
    status_code=status.HTTP_201_CREATED,
    response_class=JSONResponse,
)
def create_match(match: MatchCreate, session: Session = Depends(get_session)) -> dict[str, str]:
    """Store a reported match in the database."""

    db_match = Match(
        match_id=match.matchId,
        mode=match.mode,
        start_time=match.startTime,
        end_time=match.endTime,
        payload=match.json(),
    )
    session.add(db_match)
    try:
        session.flush()
    except IntegrityError as exc:  # pragma: no cover - defensive path
        session.rollback()
        raise HTTPException(
            status_code=status.HTTP_409_CONFLICT,
            detail="matchId already exists",
        ) from exc

    return {"matchId": match.matchId}


@app.get("/api/v1/matches")
def list_matches(
    session: Session = Depends(get_session),
    mode: str | None = Query(default=None, description="Filter by gametype"),
    limit: int = Query(default=50, ge=1, le=200),
    offset: int = Query(default=0, ge=0),
) -> dict[str, list[dict[str, object]]]:
    """Return a paginated list of matches."""

    query = select(Match).order_by(Match.start_time.desc()).offset(offset).limit(limit)
    if mode:
        query = query.where(Match.mode == mode)

    rows = session.execute(query).scalars().all()
    matches = [json.loads(row.payload) for row in rows]
    return {"matches": matches}


@app.get("/api/v1/matches/{match_id}")
def get_match(
    match_id: str = Path(..., description="The matchId that was originally submitted"),
    session: Session = Depends(get_session),
) -> dict[str, object]:
    """Return a single match payload."""

    query = select(Match).where(Match.match_id == match_id)
    result = session.execute(query).scalar_one_or_none()
    if result is None:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Match not found")

    payload = json.loads(result.payload)
    payload["createdAt"] = result.created_at.isoformat()
    return payload


@app.delete(
    "/api/v1/matches/{match_id}",
    status_code=status.HTTP_204_NO_CONTENT,
)
def delete_match(
    match_id: str = Path(..., description="The matchId to delete"),
    session: Session = Depends(get_session),
) -> JSONResponse:
    """Delete a match from the database."""

    query = select(Match).where(Match.match_id == match_id)
    result = session.execute(query).scalar_one_or_none()
    if result is None:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Match not found")

    session.delete(result)
    session.flush()
    return JSONResponse(status_code=status.HTTP_204_NO_CONTENT, content={})
