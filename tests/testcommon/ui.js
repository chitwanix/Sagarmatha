// -*- mode: js; js-indent-level: 4; indent-tabs-mode: nil -*-

const Clutter = imports.gi.Clutter;
const GLib = imports.gi.GLib;
const St = imports.gi.St;
const Sagarmatha = imports.gi.Sagarmatha;

const Environment = imports.ui.environment;

function init() {
    Environment.init();

    let stage = Clutter.Stage.get_default();
    let context = St.ThemeContext.get_for_stage (stage);
    let stylesheetPath = GLib.getenv("SAGARMATHA_TESTSDIR") + "/testcommon/test.css";
    let theme = new St.Theme ({ application_stylesheet: stylesheetPath });
    context.set_theme (theme);
}
