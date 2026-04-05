import pathlib


REPO_ROOT = pathlib.Path(__file__).resolve().parents[1]
G_ACTIVE = REPO_ROOT / "engine" / "code" / "game" / "g_active.c"


def _load_g_active() -> str:
    return G_ACTIVE.read_text(encoding="utf-8")


def test_intro_path_runs_before_follow_observe_paths() -> None:
    source = _load_g_active()

    intro_guard = "if ( G_ApplyIntroCamSequence( ent ) ) {\n\t\treturn;\n\t}"
    follow_branch = "if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW ) {"
    observe_branch = "else if ( ent->client->sess.spectatorState == SPECTATOR_OBSERVE )"

    intro_index = source.find(intro_guard)
    follow_index = source.find(follow_branch)
    observe_index = source.find(observe_branch)

    assert intro_index != -1, "Intro camera early-return guard missing in SpectatorClientEndFrame."
    assert follow_index != -1, "SPECTATOR_FOLLOW branch missing in SpectatorClientEndFrame."
    assert observe_index != -1, "SPECTATOR_OBSERVE branch missing in SpectatorClientEndFrame."
    assert intro_index < follow_index < observe_index


def test_observer_spot_functions_remain_on_normal_spectator_path() -> None:
    source = _load_g_active()

    # During normal SPECTATOR_OBSERVE flow, observer spot updates still run each frame.
    assert "if (client->sess.spectatorState == SPECTATOR_OBSERVE)" in source
    assert "UpdateObserverSpot( ent, qfalse );" in source

    # Explicit transition logic still uses FindBestObserverSpot when switching
    # from free/follow to observe.
    assert "if ( FindBestObserverSpot(ent, &g_entities[client->sess.spectatorClient], origin, angles) )" in source


def test_race_observer_fallbacks_stay_explicit() -> None:
    source = _load_g_active()

    follow_fallback = (
        "if ( isRaceObserver( ent->s.number ) ) {\n"
        "\t\t\t\tent->client->ps.pm_flags &= ~PMF_FOLLOW;\n"
        "\t\t\t\tent->client->sess.spectatorState = SPECTATOR_FOLLOW;"
    )
    observe_fallback = (
        "if ( isRaceObserver( ent->s.number ) ) {\n"
        "\t\t\t\tent->client->ps.pm_flags &= ~PMF_OBSERVE;\n"
        "\t\t\t\tent->client->sess.spectatorState = SPECTATOR_FOLLOW;"
    )

    assert follow_fallback in source
    assert observe_fallback in source
