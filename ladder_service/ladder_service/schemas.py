"""Pydantic schemas for request and response payloads."""
from __future__ import annotations

from datetime import datetime
from typing import Any, Literal, Optional, get_args

from pydantic import BaseModel, Field, HttpUrl, root_validator, validator


Gametype = Literal[
    "GT_RACING",
    "GT_RACING_DM",
    "GT_SPRINT",
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
_TEAM_MODES: set[str] = {
    "GT_TEAM_RACING",
    "GT_TEAM_RACING_DM",
    "GT_TEAM",
    "GT_CTF",
    "GT_CTF4",
    "GT_DOMINATION",
}


def _mode_error(code: str, message: str) -> ValueError:
    return ValueError(f"[{code}] {message}")


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

    @root_validator
    def validate_mode_specific_payload(cls, values: dict[str, Any]) -> dict[str, Any]:
        mode = values.get("mode")
        players = values.get("players") or []
        teams = values.get("teams")

        if mode not in _TEAM_MODES:
            if teams:
                raise _mode_error(
                    "MODE_FORBIDS_TEAMS",
                    f"mode '{mode}' forbids teams payload",
                )
            for player in players:
                if player.team is not None:
                    player.team = None
            values["teams"] = None
            return values

        if not teams:
            raise _mode_error(
                "MODE_REQUIRES_TEAMS",
                f"mode '{mode}' requires a non-empty teams array",
            )

        team_names: set[str] = set()
        for team in teams:
            normalized_name = team.team.strip().lower()
            if not normalized_name:
                raise _mode_error("TEAM_NAME_INVALID", "team name must not be blank")
            if normalized_name in team_names:
                raise _mode_error(
                    "TEAM_DUPLICATE",
                    f"duplicate team '{team.team}' in teams payload",
                )
            team.team = normalized_name
            team_names.add(normalized_name)

        for player in players:
            if player.team is None:
                raise _mode_error(
                    "PLAYER_TEAM_REQUIRED",
                    f"player '{player.playerId}' is missing team in mode '{mode}'",
                )
            normalized_player_team = str(player.team).strip().lower()
            if normalized_player_team not in team_names:
                raise _mode_error(
                    "PLAYER_TEAM_UNKNOWN",
                    f"player '{player.playerId}' references unknown team '{player.team}'",
                )
            player.team = normalized_player_team

        return values

    class Config:
        extra = "allow"


class MatchRead(MatchCreate):
    id: int
    createdAt: datetime

    class Config:
        orm_mode = True
