<?php
/**
 * Q3Rally Ladder Service – Version & Changelog
 * version.php
 */

declare(strict_types=1);

const LADDER_VERSION = '1.0.5';

const LADDER_CHANGELOG = [
    '1.0.5' => [
        'date'    => '2026-03-27',
        'changes' => [
            'Admin: added Offline tab – keys registered via the in-game wizard are now grouped separately',
            'Admin: added Revoked tab with permanent delete action',
            'Admin: Offline keys can be approved/revoked directly from the Offline tab',
            'register.php: JSON response branch for in-game registration wizard (Accept: application/json)',
            'Engine: ladder_register command now async (non-blocking, driven by SV_LadderFrame)',
            'Engine: sv_ladderApiKey, sv_ladderEnabled, sv_hostname set from engine code after registration',
            'Engine: writeconfig triggered automatically on successful registration',
            'Engine: SV_LadderFrame called before com_sv_running guard so registration works in main menu',
        ],
    ],
    '1.0.4' => [
        'date'    => '2026-03-20',
        'changes' => [
            'In-game ladder registration wizard (ui_rally_ladder_wizard)',
            'ladder_register / ladder_register_abort console commands',
            'Per-server API key authentication via Bearer token',
            'Auto-suspend inactive keys after 90 days',
            'Admin panel: approve / revoke keys',
            'register.php: self-service server key registration',
        ],
    ],
    '1.0.3' => [
        'date'    => '2026-02-10',
        'changes' => [
            'Player profile overlay in leaderboard frontend',
            'Achievement tier display per category',
            'Match detail overlay',
            'Online / Offline source toggle in leaderboard',
        ],
    ],
    '1.0.2' => [
        'date'    => '2026-01-15',
        'changes' => [
            'Leaderboard index endpoint (/matches/index) for fast frontend loads',
            'TGA levelshot support in frontend',
            'Map metadata from .arena files',
            'Levelshot manifest endpoint (/maps/levelshots)',
        ],
    ],
    '1.0.1' => [
        'date'    => '2025-12-01',
        'changes' => [
            'Rate limiting per IP (30 req / 60 s)',
            'Match spool with retry and exponential backoff',
            'Elimination, CTF and Deathmatch leaderboard tabs',
        ],
    ],
    '1.0.0' => [
        'date'    => '2025-11-01',
        'changes' => [
            'Initial release',
            'Match upload endpoint (POST /matches)',
            'Basic leaderboard frontend',
            'Race / Sprint leaderboard',
        ],
    ],
];
