"""Position-based progress must distinguish sliding from stationary yaw projection."""
import pathlib
import runpy
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]


def test_position_progress_metrics() -> None:
    summarize = runpy.run_path(str(ROOT / "tools" / "bot_dm_metrics.py"))["summarize"]
    with tempfile.TemporaryDirectory() as temp:
        log = pathlib.Path(temp) / "match.log"
        rows = []
        for tick in range(9):
            for client in [0, 1]:
                rows.append(f'BotDM: time={tick*500} client={client} enemy=2 speed=0 '
                            f'firing=0 aimError=0 x=0 y={tick*50 if client else 0} z=16 recovery=0')
        log.write_text('\n'.join(rows))
        stats = summarize(log)
        assert stats['stationary_sample_share'] == 1
        assert stats['low_position_progress_episodes_at_least_2s'] == 1
        assert stats['longest_low_position_progress_seconds'] == 4
        assert 0 < stats['low_position_progress_sample_share'] < 0.5
        # A logging gap cannot be counted as continuous lack of progress.
        log.write_text('\n'.join([
            f'BotDM: time={time} client=0 enemy=-1 speed=0 firing=0 x=0 y=0 z=16'
            for time in [0, 500, 4000, 4500]
        ]))
        stats = summarize(log)
        assert stats['low_position_progress_episodes_at_least_2s'] == 0

