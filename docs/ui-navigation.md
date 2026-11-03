# Menu navigation convention

Regular menus place Back at virtual x=40, y=420, width=120, height=24.
Call `Frontend_SetBackButton` after adding the item and after any layout update.
The helper preserves the action callback and hidden/disabled flags, and assigns
both drawing and hit bounds. It supports legacy bitmap and modern text items.

Back remains on the left; primary actions such as Apply, Start or Save belong
on the right. The Config hub places Reset defaults on the right. Dialog answer
buttons (Save/Discard/Cancel, Yes/No) retain their grouped dialog layout.

In-game follow labels, spectator hints and centerprints use the ingame font
atlas. Centerprints retain their fade, multiline handling and screen placement.

Manual verification: Config and its tabs, graphics pages, driver info, player
settings and vehicle selection, server browser and direct connection, race setup,
bot selection/add/remove, Rivals, Downloads and Demos. Check mouse hover/click,
Escape, disabled actions, and layout changes such as pagination and bot counts.
