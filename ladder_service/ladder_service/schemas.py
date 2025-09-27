"""Pydantic schemas for request and response payloads."""
from __future__ import annotations

from datetime import datetime
from typing import Any, Literal, Optional

from pydantic import BaseModel, Field, HttpUrl, validator


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
    team: Optional[str]
    rawScore: int
    totalTime: Optional[str]
    position: Optional[int]
    damageDealt: Optional[int]
    damageTaken: Optional[int]

    class Config:
        extra = "allow"


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


class MatchRead(MatchCreate):
    id: int
    createdAt: datetime

    class Config:
        orm_mode = True
