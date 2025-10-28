"""Pydantic schemas for request and response payloads."""
from __future__ import annotations

from datetime import datetime
from typing import Any, Literal, Optional, get_args

from pydantic import BaseModel, Field, HttpUrl, root_validator, validator


Gametype = Literal[
    "GT_RACING",
    "GT_RACING_DM",
    "GT_TEAM_RACING",
    "GT_TEAM_RACING_DM",
    "GT_ELIMINATION",
    "GT_LCS",
    "GT_DERBY",
    "GT_DEATHMATCH",
    "GT_TEAM",
    "GT_CTF",
    "GT_CTF4",
    "GT_DOMINATION",
    "GT_SINGLE_PLAYER",
]


_VALID_GAMETYPES: set[str] = set(get_args(Gametype))


class ServerInfo(BaseModel):
    name: str = Field(..., description="Human readable server name")
    host: str = Field(..., description="Host:port of the reporting server")
    build: Optional[str] = Field(
        default=None, description="Build identifier reported by the server"
    )


class Settings(BaseModel):
    g_gametype: int

    class Config:
        extra = "allow"


class Player(BaseModel):
    playerId: str
    displayName: Optional[str]
    team: Optional[str | int]
    rawScore: Optional[int]
    score: Optional[int]
    totalTime: Optional[str]
    position: Optional[int]
    damageDealt: Optional[int]
    damageTaken: Optional[int]

    class Config:
        extra = "allow"

    @root_validator(pre=True)
    def ensure_scores(cls, values: dict[str, Any]) -> dict[str, Any]:
        raw = values.get("rawScore")
        score = values.get("score")
        if raw is None and score is None:
            raise ValueError("Player payload must provide rawScore or score")
        if raw is None:
            values["rawScore"] = score
        elif score is None:
            values["score"] = raw
        return values


class Team(BaseModel):
    team: str
    rawScore: Optional[int]
    normalizedScore: Optional[float]

    class Config:
        extra = "allow"


class Event(BaseModel):
    timestamp: datetime
    type: str

    class Config:
        extra = "allow"


class MatchCreate(BaseModel):
    matchId: str
    mode: Gametype
    startTime: datetime
    endTime: datetime
    duration: Optional[str]
    map: Optional[str]
    server: Optional[ServerInfo]
    serverUrl: Optional[HttpUrl] = Field(
        default=None, description="Optional public URL of the reporting server"
    )
    settings: Settings
    players: list[Player] = Field(..., min_items=1)
    teams: list[Team] | None = None
    events: list[Event] | None = None

    @validator("players")
    def ensure_player_ids(cls, value: list[Player]) -> list[Player]:
        missing_id = any(not player.playerId for player in value)
        if missing_id:
            raise ValueError("All players must provide a playerId")
        return value

    @validator("mode", pre=True)
    def normalize_mode(cls, value: object) -> str:
        if isinstance(value, str):
            candidate = value.strip()
            if candidate:
                upper = candidate.upper().replace(" ", "_")
                if not upper.startswith("GT_"):
                    upper = f"GT_{upper}"
                if upper in _VALID_GAMETYPES:
                    return upper
        return "GT_ELIMINATION"

    class Config:
        extra = "allow"


class MatchRead(MatchCreate):
    id: int
    createdAt: datetime

    class Config:
        orm_mode = True
