Q3Rally — *It's damn fast, baby*
===============================


Q3Rally is a standalone game based on ioquake3.

For compiling, see [engine/README.md](engine/README.md).

## Elimination Mode

Elimination races periodically remove the driver at the back of the pack until
only one racer remains. After the start lights go green, the server waits the
duration defined in `g_eliminationStartDelay` before scheduling the first
elimination (default `30000`, i.e., 30 seconds). Once active, eliminations are
triggered every `g_eliminationInterval` milliseconds (default `15000`, or 15
seconds). Drivers receive an on-screen warning `g_eliminationWarning`
milliseconds before each cut (default `5000`, or 5 seconds); set this to `0` if
you want an immediate drop with no countdown.

These CVars can be adjusted in your server configuration to tune pacing for
public servers or competitive events. For a step-by-step server setup walkthrough
see the [Q3Rally Dedicated Server Setup Guide](readme/q3rally_dedicated_server_guide.txt).

## Vehicle balancing

Servers can enforce basic balancing rules for custom vehicles. Two cvars
control the allowed spread of key vehicle attributes:

* `g_vehicleHpMaxRatio` – maximum allowed ratio between the highest and
  lowest horsepower peak among all cars (default `1.2`).
* `g_vehicleHealthMaxRatio` – maximum allowed ratio between the highest
  and lowest maximum health (default `1.5`).

Typical vehicles ship with an `hpPeak` around 320 and `maxHealth` near 100.
Allowing a ratio between `1.0`–`1.5` for horsepower and `1.0`–`2.0` for
health keeps racing competitive while still permitting variety. Admins are
free to tweak these cvars to fit their custom vehicle sets.

## Resources

* [Q3Rally Website](http://www.q3rally.com)
* [Q3Rally on ModDB](https://www.moddb.com/games/q3rally)
* [Q3Rally on Discord](https://discord.gg/rX8Sxmh)

## License
The source code (engine directory) is licensed under the GPLv2 or later unless specified otherwise.

The data files (baseq3r directory) do not have have a known license and should be treated as non-commericial / non-free.
