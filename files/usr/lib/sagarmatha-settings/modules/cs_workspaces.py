#!/usr/bin/env python

from SettingsWidgets import *

class Module:
    def __init__(self, content_box):
        keywords = _("workspace, osd, expo, monitor")
        advanced = True
        sidePage = SidePage(_("Workspaces"), "workspaces.svg", keywords, advanced, content_box)
        self.sidePage = sidePage
        self.name = "workspaces"
        self.category = "prefs"
        sidePage.add_widget(GSettingsCheckButton(_("Enable workspace OSD"), "org.sagarmatha", "workspace-osd-visible", None))

        box = IndentedHBox()
        box.add(GSettingsSpinButton(_("Workspace OSD duration"), "org.sagarmatha", "workspace-osd-duration", "org.sagarmatha/workspace-osd-visible", 0, 2000, 50, 400, _("milliseconds")))
        sidePage.add_widget(box, True)

        box = IndentedHBox()
        box.add(GSettingsSpinButton(_("Workspace OSD horizontal position"), "org.sagarmatha", "workspace-osd-x", "org.sagarmatha/workspace-osd-visible", 0, 100, 5, 50, _("percent of the monitor's width")))
        sidePage.add_widget(box, True)

        box = IndentedHBox()
        box.add(GSettingsSpinButton(_("Workspace OSD vertical position"), "org.sagarmatha", "workspace-osd-y", "org.sagarmatha/workspace-osd-visible", 0, 100, 5, 50, _("percent of the monitor's height")))
        sidePage.add_widget(box, True)

        sidePage.add_widget(GSettingsCheckButton(_("Allow cycling through workspaces"), "org.sagarmatha.muffin", "workspace-cycle", None), True)
        sidePage.add_widget(GSettingsCheckButton(_("Only use workspaces on primary monitor (requires Sagarmatha restart)"), "org.sagarmatha.muffin", "workspaces-only-on-primary", None), True)
        sidePage.add_widget(GSettingsCheckButton(_("Display Expo view as a grid"), "org.sagarmatha", "workspace-expo-view-as-grid", None))

    def shouldLoad(self):
        return True

