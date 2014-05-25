#!/usr/bin/env python

from SettingsWidgets import *

class Module:
    def __init__(self, content_box):
        keywords = _("logging, click, notifications")
        advanced = True
        sidePage = SidePage(_("General"), "general.svg", keywords, advanced, content_box)
        self.sidePage = sidePage
        self.name = "general"
        self.category = "prefs"
        sidePage.add_widget(GSettingsCheckButton(_("Log LookingGlass output to ~/.sagarmatha/glass.log (Requires Sagarmatha restart)"), "org.sagarmatha", "enable-looking-glass-logs", None), True)
        sidePage.add_widget(GSettingsCheckButton(_("Display notifications"), "org.sagarmatha", "display-notifications", None))

