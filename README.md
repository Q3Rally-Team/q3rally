Q3Rally — *It's damn fast, baby*
===============================


Q3Rally is a standalone game based on ioquake3.

For compiling, see [engine/README.md](engine/README.md).

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
