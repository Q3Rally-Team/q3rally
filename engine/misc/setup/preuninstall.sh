#!/bin/sh
rmdir --ignore-fail-on-non-empty baseq3r >/dev/null 2>&1
if test -e "$SETUP_INSTALLPATH"/q3rally.desktop.in; then
  xdg_desktop_menu=`which xdg-desktop-menu 2>/dev/null`
  if test "x$xdg_desktop_menu" = x; then
    xdg_desktop_menu=./xdg-desktop-menu
  fi
  $xdg_desktop_menu uninstall --novendor q3rally.desktop
  rm q3rally.desktop
fi
