Q3Rally - *It's damn fast, baby*
================================

Q3Rally is a standalone game based on ioquake3. This repository contains the
Q3Rally engine changes, game data, project documentation, tests, and the
optional Ladder service used for match reporting.

## Quick Links

* Engine build instructions: [engine/README.md](engine/README.md)
* Ladder payload contract: [docs/ladder_payload_semantics.md](docs/ladder_payload_semantics.md)
* Ladder E2E mode matrix: [docs/e2e_mode_test_matrix.md](docs/e2e_mode_test_matrix.md)
* Python Ladder service: [ladder_service/README.md](ladder_service/README.md)
* PHP Ladder webservice: [ladder_service/php_webservice/README.md](ladder_service/php_webservice/README.md)
* CTF4 announcer asset notes: [baseq3r/sound/teamplay/README.md](baseq3r/sound/teamplay/README.md)

## Repository Layout

* `engine/` - Q3Rally's ioquake3-derived engine source and build scripts.
* `baseq3r/` - game data, scripts, bots, sounds, and asset placeholders.
* `docs/` - project documentation and Ladder payload specifications.
* `ladder_service/` - Python and PHP implementations of the match reporting service.
* `tests/` - acceptance notes and test documentation.

## Resources

* [Q3Rally Website](http://www.q3rally.com)
* [Q3Rally on ModDB](https://www.moddb.com/games/q3rally)
* [Q3Rally on Discord](https://discord.gg/rX8Sxmh)

## License

The source code in `engine/` is licensed under the GPLv2 or later unless
specified otherwise.

The data files in `baseq3r/` do not have a known license and should be treated
as non-commercial / non-free.
