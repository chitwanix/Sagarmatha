const Applet = imports.ui.applet;
const Sagarmatha = imports.gi.Sagarmatha;
const GLib = imports.gi.GLib;
const GTop = imports.gi.GTop;
const Gio = imports.gi.Gio;
const Lang = imports.lang;
const Mainloop = imports.mainloop;
const PopupMenu = imports.ui.popupMenu;
const St = imports.gi.St;
const NetworkManager = imports.gi.NetworkManager;
const Main = imports.ui.main;
const Settings = imports.ui.settings; // Needed for settings API
const Clutter = imports.gi.Clutter; // Needed for vnstat addition
const ModalDialog = imports.ui.modalDialog; // Needed for Modal Dialog used in Alert
const NMClient = imports.gi.NMClient; // Needed for modifications to NM calls

// Alert response using a Modal Dialog - approach thanks to Mark Bolin 

function AlertDialog(value) {
    this._init(value);
}
AlertDialog.prototype = {
    __proto__: ModalDialog.ModalDialog.prototype,
    _init: function (value) {
        ModalDialog.ModalDialog.prototype._init.call(this);
        let label = new St.Label({
            text: value ,
            style_class: "centered"
        });
        this.contentLayout.add(label);
        this.setButtons([{
            style_class: "centered",
            label: "Ok",
            action: Lang.bind(this, function () {
                this.close();
            })
        }]);
    },
}

function MyApplet(metadata, orientation, panel_height, instance_id) {
    this._init(metadata, orientation, panel_height, instance_id);
};

MyApplet.prototype = {
    __proto__: Applet.Applet.prototype,
    _init: function (metadata, orientation, panel_height, instance_id) {
        Applet.Applet.prototype._init.call(this, orientation, panel_height, instance_id);
        try {

            this.UUID = metadata.uuid // Pick up UUID from metadata to make everything location independent
            this.settings = new Settings.AppletSettings(this, metadata.uuid, instance_id);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "refreshInterval-spinner",
                "refreshIntervalIn",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "decimalsToShow-spinner",
                "decimalsToShowIn",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.BIDIRECTIONAL,
                "monitored-interface",
                "monitoredIinterfaceBi",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "useDefaultInterface",
                "useDefaultInterfaceIn",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "defaultInterface",
                "defaultInterfaceIn",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "useTotalLimit",
                "useTotalLimit",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "totalLimit",
                "totalLimit",
                this.on_alert_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.BIDIRECTIONAL,
                "alertPercentage",
                "alertPercentage",
                this.on_alert_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "cumulativeInterface1",
                "cumulativeInterface1",
                this.on_interface_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "cumulativeComment1",
                "cumulativeComment1",
                this.on_interface_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.BIDIRECTIONAL,
                "cumulativeTotal1",
                "cumulativeTotal1",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "cumulativeInterface2",
                "cumulativeInterface2",
                this.on_interface_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "cumulativeComment2",
                "cumulativeComment2",
                this.on_interface_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.BIDIRECTIONAL,
                "cumulativeTotal2",
                "cumulativeTotal2",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "cumulativeInterface3",
                "cumulativeInterface3",
                this.on_interface_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "cumulativeComment3",
                "cumulativeComment3",
                this.on_interface_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.BIDIRECTIONAL,
                "cumulativeTotal3",
                "cumulativeTotal3",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "useVnstat",
                "useVnstat",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.BIDIRECTIONAL,
                "compactDisplay",
                "compactDisplay",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "appletWidthSetting",
                "appletWidthSetting",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "useCrisisManagement",
                "useCrisisManagement",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "crisisManagement",
                "crisisManagement",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "disconnectDelay",
                "disconnectDelay",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "useAlertSound",
                "useAlertSound",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "alertSound",
                "alertSound",
                this.on_settings_changed,
                null);
            this.settings.bindProperty(Settings.BindingDirection.IN,
                "displayExtraHousekeeping",
                "displayExtraHousekeeping",
                this.on_options_changed,
                null);

            this.cssfile = metadata.path + "/stylesheet.css";
            this.changelog = metadata.path + "/changelog.txt";
            this.helpfile = metadata.path + "/helpfile.txt";
            this.crisisScript = metadata.path + "/crisisScript";
            this.appletPath = metadata.path;
            this.UUID = metadata.uuid;

            this._client = NMClient.Client.new(); //++

			this.abortFlag = true;

            // Set up display in applet
            if (!this.compactDisplay) {
                this.appletWidth = this.appletWidthSetting;
            } else {
                this.appletWidth = (this.appletWidthSetting / 2) + 1;
            }

            this.labelOne = new St.Label({
                reactive: true,
                track_hover: true,
                style_class: "numa-left"
            });
            this.labelTwo = new St.Label({
                reactive: true,
                track_hover: true,
                style_class: "numa-right"
            });
            this.actor.add(this.labelOne, {
                y_align: St.Align.MIDDLE,
                y_fill: false
            });
            this.actor.add(this.labelTwo, {
                y_align: St.Align.MIDDLE,
                y_fill: false
            });

            // Set up left click menu
            this.menuManager = new PopupMenu.PopupMenuManager(this);
            this.menu = new Applet.AppletPopupMenu(this, orientation);
            this.menuManager.addMenu(this.menu);

            // Add code for vnstat display
            this.imageWidget = new St.Bin({
                x_align: St.Align.MIDDLE
            }); // As per Clem's code
            this.textWidget = new St.Label(); // Used to display error message if vnstat not loaded
            this.mainBox = new St.BoxLayout({
                vertical: true
            }); // Extra mainBox to enable hiding of vnstati output
            this.vnstatImage = metadata.path + "/vnstatImage.png"; // path to image file in applet's folder

            this.makeMenu();

            // Initial conditions
            this.gtop = new GTop.glibtop_netload();
            this.timeOld = GLib.get_monotonic_time();
            this.upOld = 0;
            this.downOld = 0;
            this.upOldC1 = 0;
            this.downOldC1 = 0;
            this.upOldC2 = 0;
            this.downOldC2 = 0;
            this.upOldC3 = 0;
            this.downOldC3 = 0;

            this.last_numa_style = 'numa-not-not-connected'; 

            this.monitoredInterfaceName = null;
            let lastUsedInterface = this.monitoredIinterfaceBi;

            this.set_applet_tooltip("No Interface being Monitored - right click to select");
            if (this.useDefaultInterfaceIn) {
                this.setMonitoredInterface(this.defaultInterfaceIn);
            }
            if (this.isInterfaceAvailable(lastUsedInterface) || lastUsedInterface == "ppp0") {
                this.setMonitoredInterface(lastUsedInterface);
            }
            this.rebuildFlag = true;
            this.on_settings_changed();
            this.update();
        } catch (e) {
            global.logError(e);
        }
    },

    on_settings_changed: function () {
        if (this.useTotalLimit) {
            this.slider_demo.setValue(this.alertPercentage / 100);
        }
        if (!this.compactDisplay) {
            this.appletWidth = this.appletWidthSetting;
        } else {
            this.appletWidth = (this.appletWidthSetting / 2) + 1;
        }
        this.updateLeftMenu();
    },

    on_alert_settings_changed: function () {
        this.makeMenu();
        this.on_settings_changed
    },

    on_interface_settings_changed: function () {
        this.makeMenu();
        this.on_settings_changed
    },

    on_options_changed: function () {
        this.buildContextMenu();
        this.makeMenu();
        this.on_settings_changed
    },
    on_slider_changed: function (slider, value) {
        this.alertPercentage = value * 100;
        this.updateLeftMenu();
    },

    getInterfaces: function () {
        return this._client.get_devices(); //++
    },

    isInterfaceAvailable: function (name) {
        let interfaces = this.getInterfaces();
        if (interfaces != null) {
            for (let i = 0; i < interfaces.length; i++) {
                let iname = interfaces[i].get_iface();
                if (iname == name && interfaces[i].state == NetworkManager.DeviceState.ACTIVATED) {
                    return true;
                }
            }
        }
        return false;
    },

    // Build left click menu - some menu items are placeholders which are updated on changes and some are conditional
    makeMenu: function () {
        this.menu.removeAll();
        // fudge to remove display of vnstat by using Box and removing actor from it as removeAll does not work on it
        this.mainBox.add_actor(this.imageWidget);
        this.mainBox.remove_actor(this.imageWidget);
        // Add code for vnstat display 
        if (this.useVnstat) {
            //			this.menu.addActor(this.imageWidget);      // Old way with actor added directly to menu
            try {
                this.menu.addActor(this.mainBox); // Now add mainbox
                this.mainBox.add_actor(this.imageWidget); // and add actor for to it which can be removed
                this.mainBox.add_actor(this.textWidget); // and text actor to handle case where vnstat not installed

                GLib.spawn_command_line_sync('vnstati -s -ne -i ' + this.monitoredInterfaceName + ' -o ' + this.vnstatImage);
                let l = new Clutter.BinLayout();
                let b = new Clutter.Box();
                let c = new Clutter.Texture({
                    keep_aspect_ratio: true,
                    filter_quality: 2,
                    filename: this.vnstatImage
                });
                b.set_layout_manager(l);
                b.add_actor(c);
                this.imageWidget.set_child(b);
            } catch (e) {
                this.textWidget.set_text(" ERROR: Please make sure vnstat and vnstati are installed and that the vnstat daemon is running! ");
                global.logError(e);
            }
            this.menuitemHead0 = new PopupMenu.PopupMenuItem("NOTE: The last time the network traffic statistics were updated by vnStat is at the top right", {
                reactive: false
            });
            this.menu.addMenuItem(this.menuitemHead0);
            this.menu.addMenuItem(new PopupMenu.PopupSeparatorMenuItem())
        }

        // Now the Cumulative Usage monitors
        this.menuitemHead1 = new PopupMenu.PopupMenuItem("Cumulative Data Usage Information", {
            reactive: false
        });
        this.menu.addMenuItem(this.menuitemHead1);

        if (this.cumulativeInterface1 != "null" && this.cumulativeInterface1 != "") {
            this.menuitemInfo1 = new PopupMenu.PopupMenuItem("Cumulative data placeholder 1", {
                reactive: false
            });
            this.menu.addMenuItem(this.menuitemInfo1);
            this.menuitemInfo3 = new PopupMenu.PopupMenuItem("       " + this.cumulativeComment1, {
                reactive: false
            });
            this.menu.addMenuItem(this.menuitemInfo3);
        }
        if (this.cumulativeInterface2 != "null" && this.cumulativeInterface2 != "") {
            this.menuitemInfo4 = new PopupMenu.PopupMenuItem("Cumulative data placeholder 2", {
                reactive: false
            });
            this.menu.addMenuItem(this.menuitemInfo4);
            this.menuitemInfo5 = new PopupMenu.PopupMenuItem("       " + this.cumulativeComment2, {
                reactive: false
            });
            this.menu.addMenuItem(this.menuitemInfo5);
        }
        if (this.cumulativeInterface3 != "null" && this.cumulativeInterface3 != "") {
            this.menuitemInfo6 = new PopupMenu.PopupMenuItem("Cumulative data placeholder 3", {
                reactive: false
            });
            this.menu.addMenuItem(this.menuitemInfo6);
            this.menuitemInfo7 = new PopupMenu.PopupMenuItem("       " + this.cumulativeComment3, {
                reactive: false
            });
            this.menu.addMenuItem(this.menuitemInfo7);
        }

        this.menu.addMenuItem(new PopupMenu.PopupSeparatorMenuItem())
        this.menuitemHead2 = new PopupMenu.PopupMenuItem("Current Connection and Interface Information", {
            reactive: false
        });
        this.menu.addMenuItem(this.menuitemHead2);

        if (this.monitoredInterfaceName != null) {
            this.menuitemInfo = new PopupMenu.PopupMenuItem("placeholder", {
                reactive: false
            });
            this.menu.addMenuItem(this.menuitemInfo);
            this.menuitemInfo.label.text = "    " + this.monitoredInterfaceName + " - Downloaded: " + this.formatSentReceived(this.downOld) + " - Uploaded: " + this.formatSentReceived(this.upOld);
        } else {
            this.menuitemInfo = new PopupMenu.PopupMenuItem("No network monitored. Please select one right-clicking the applet.", {
                reactive: false
            });
            this.menu.addMenuItem(this.menuitemInfo);
        }
        //	Slider only if Alerts enabled
        if (this.useTotalLimit) {
            this.menuitemInfo2 = new PopupMenu.PopupMenuItem("     Note: Alerts not enabled in Settings", {
                reactive: false
            });
            this.menu.addMenuItem(this.menuitemInfo2);
            this.slider_demo = new PopupMenu.PopupSliderMenuItem(0);
            this.slider_demo.connect("value-changed", Lang.bind(this, this.on_slider_changed));
            this.menu.addMenuItem(this.slider_demo);
        }
        this.updateLeftMenu(); // Updates values where placeholder used.
        this.on_settings_changed();
    },

    // Update left menu
    updateLeftMenu: function () {

        if (this.useTotalLimit) {
            this.menuitemInfo2.label.text = "    " + "Alert level (Orange): " + Math.round(this.alertPercentage) + " % of Data Limit of " + this.totalLimit + " MBytes.";
        }
        if (this.cumulativeInterface1 != "null" && this.cumulativeInterface1 != "") {
            this.menuitemInfo1.label.text = "   " + this.cumulativeInterface1 + " - Cumulative Data Usage: " + this.formatSentReceived(this.cumulativeTotal1 * 1024 * 1024);
        }
        if (this.cumulativeInterface2 != "null" && this.cumulativeInterface2 != "") {
            this.menuitemInfo4.label.text = "   " + this.cumulativeInterface2 + " - Cumulative Data Usage: " + this.formatSentReceived(this.cumulativeTotal2 * 1024 * 1024);
        }
        if (this.cumulativeInterface3 != "null" && this.cumulativeInterface3 != "") {
            this.menuitemInfo6.label.text = "   " + this.cumulativeInterface3 + " - Cumulative Data Use: " + this.formatSentReceived(this.cumulativeTotal3 * 1024 * 1024);
        }
    },

    // Build right click context menu
    buildContextMenu: function () {
        this._applet_context_menu.removeAll();
        this._applet_context_menu.addMenuItem(new PopupMenu.PopupMenuItem("Select a network manager interface to be monitored:", {
            reactive: false
        }));

        let interfaces = this.getInterfaces();
        if (interfaces != null) {
            for (let i = 0; i < interfaces.length; i++) {
                let name = interfaces[i].get_iface();
                let displayname = "\t" + name;
                if (this.isInterfaceAvailable(name)) {
                    displayname = displayname + " (Active)";
                }
                if (this.monitoredInterfaceName == name) {
                    displayname = "\u2714" + displayname;
                }
                let menuitem = new PopupMenu.PopupMenuItem(displayname);
                menuitem.connect('activate', Lang.bind(this, function () {
                    this.setMonitoredInterface(name);
                }));
                this._applet_context_menu.addMenuItem(menuitem);
            }
        }

        this._applet_context_menu.addMenuItem(new PopupMenu.PopupMenuItem("or Select an independent interface to be monitored:", {
            reactive: false
        }));

        let displayname2 = "\t" + "ppp0     (for most USB Mobile Internet Modems)";
        if (this.monitoredInterfaceName == "ppp0") {
            displayname2 = "\u2714" + displayname2;
        }
        let menuitem = new PopupMenu.PopupMenuItem(displayname2);
        menuitem.connect('activate', Lang.bind(this, function () {
            this.setMonitoredInterface("ppp0");
        }));
        this._applet_context_menu.addMenuItem(menuitem)

        let menuitem = new PopupMenu.PopupMenuItem("Check for Changes in Devices and Display Options");
        menuitem.connect('activate', Lang.bind(this, function (event) {
            this.rebuildFlag = true;
        }));
        this._applet_context_menu.addMenuItem(menuitem);

        this._applet_context_menu.addMenuItem(new PopupMenu.PopupSeparatorMenuItem());

        let menuitem = new PopupMenu.PopupMenuItem("Toggle Display from \u2193 and \u2191 to \u21f5");
        menuitem.connect('activate', Lang.bind(this, function (event) {
            if (this.compactDisplay) {
                this.compactDisplay = false;
                this.appletWidth = this.appletWidthSetting;
            } else {
                this.compactDisplay = true;
                this.appletWidth = (this.appletWidthSetting / 2) + 1;
            }
        }));
        this._applet_context_menu.addMenuItem(menuitem);

        let menuitem = new PopupMenu.PopupMenuItem("Settings");
        menuitem.connect('activate', Lang.bind(this, function (event) {
            GLib.spawn_command_line_async('sagarmatha-settings applets ' + this.UUID);
        }));
        this._applet_context_menu.addMenuItem(menuitem);

        this._applet_context_menu.addMenuItem(new PopupMenu.PopupSeparatorMenuItem());

        // Set up sub menu for Housekeeping and System Items
        this.subMenu1 = new PopupMenu.PopupSubMenuMenuItem("Housekeeping and System Sub Menu");
        this._applet_context_menu.addMenuItem(this.subMenu1);

        this.subMenuItem1 = new PopupMenu.PopupMenuItem("Open System Monitor");
        this.subMenuItem1.connect('activate', Lang.bind(this, function (event) {
            GLib.spawn_command_line_async('gnome-system-monitor');
        }));
        this.subMenu1.menu.addMenuItem(this.subMenuItem1); // Note this has subMenu1.menu not subMenu1._applet_context_menu

        this.subMenuItem2 = new PopupMenu.PopupMenuItem("View the Changelog");
        this.subMenuItem2.connect('activate', Lang.bind(this, function (event) {
            GLib.spawn_command_line_async('gedit ' + this.changelog);
        }));
        this.subMenu1.menu.addMenuItem(this.subMenuItem2);
 
      this.subMenuItem3 = new PopupMenu.PopupMenuItem("Open Website for support using firefox");
        this.subMenuItem3.connect('activate', Lang.bind(this, function (event) {
            GLib.spawn_command_line_async('firefox "http://www.pcurtis.com/summit.htm#numa"');
        }));
        this.subMenu1.menu.addMenuItem(this.subMenuItem3);

        this.subMenu1.menu.addMenuItem(new PopupMenu.PopupSeparatorMenuItem());

        if (this.displayExtraHousekeeping) {
            this.subMenuItem4 = new PopupMenu.PopupMenuItem("Open stylesheet.css  (Advanced Function)");
            this.subMenuItem4.connect('activate', Lang.bind(this, function (event) {
                GLib.spawn_command_line_async('gedit ' + this.cssfile);
            }));
            this.subMenu1.menu.addMenuItem(this.subMenuItem4);

            this.subMenuItem8 = new PopupMenu.PopupMenuItem("Open alertScript  (Advanced Function)");
            this.subMenuItem8.connect('activate', Lang.bind(this, function (event) {
                GLib.spawn_command_line_async('gedit ' + this.appletPath + '/alertScript');
            }));
            this.subMenu1.menu.addMenuItem(this.subMenuItem8);

            this.subMenuItem5 = new PopupMenu.PopupMenuItem("Open suspendScript  (Advanced Function)");
            this.subMenuItem5.connect('activate', Lang.bind(this, function (event) {
                GLib.spawn_command_line_async('gedit ' + this.appletPath + '/suspendScript');
            }));
            this.subMenu1.menu.addMenuItem(this.subMenuItem5);

            this.subMenu1.menu.addMenuItem(new PopupMenu.PopupSeparatorMenuItem());

            this.subMenuItem9 = new PopupMenu.PopupMenuItem("Test alertScript  (Advanced Test Function)");
            this.subMenuItem9.connect('activate', Lang.bind(this, function (event) {
                GLib.spawn_command_line_async('sh ' + this.appletPath + '/alertScript');
            }));
            this.subMenu1.menu.addMenuItem(this.subMenuItem9);

            this.subMenuItem6 = new PopupMenu.PopupMenuItem("Test suspendScript  (Advanced Test Function)");
            this.subMenuItem6.connect('activate', Lang.bind(this, function (event) {
                GLib.spawn_command_line_async('sh ' + this.appletPath + '/suspendScript');
            }));
            this.subMenu1.menu.addMenuItem(this.subMenuItem6);

            this.subMenuItem7 = new PopupMenu.PopupMenuItem("Test Crisis Management Function (Advanced Test Function)");
            this.subMenuItem7.connect('activate', Lang.bind(this, function (event) {
                this.crisis();
            }));
            this.subMenu1.menu.addMenuItem(this.subMenuItem7);
         }
    },

    setMonitoredInterface: function (name) {
        this.monitoredInterfaceName = name;
        this.rebuildFlag = true;
        // This is a convenient place to ensure upOld and downOld are reset after change of interface or start-up
        GTop.glibtop_get_netload(this.gtop, this.monitoredInterfaceName);
        this.upOld = this.gtop.bytes_out;
        this.downOld = this.gtop.bytes_in;

        GTop.glibtop_get_netload(this.gtop, this.cumulativeInterface1);
        this.upOldC1 = this.gtop.bytes_out;
        this.downOldC1 = this.gtop.bytes_in;

        GTop.glibtop_get_netload(this.gtop, this.cumulativeInterface2);
        this.upOldC2 = this.gtop.bytes_out;
        this.downOldC2 = this.gtop.bytes_in;

        GTop.glibtop_get_netload(this.gtop, this.cumulativeInterface3);
        this.upOldC3 = this.gtop.bytes_out;
        this.downOldC3 = this.gtop.bytes_in;

        this.monitoredIinterfaceBi = name; // save using sagarmatha settings
    },

    on_applet_clicked: function (event) {
        if (!this.abortFlag) {
            this.abortFlag = true;   // Set flag to abort any delayed activities and do not pop up menu
        } else {
               if (!this.menu.isOpen) {
                     this.makeMenu();
                }
               this.menu.toggle();
        }
    },

    playAlert: function(){
        if(this.useAlertSound) {
            GLib.spawn_command_line_async('play ' + this.alertSound);
//            GLib.spawn_command_line_async('play /usr/share/sounds/freedesktop/stereo/alarm-clock-elapsed.oga');
        }
    },

     delayedNetworkingDisable: function (delay) {
            GLib.spawn_command_line_async('zenity --warning --text="THE DATA USAGE LIMIT HAS BEEN EXCEEDED\n\nAll network connections Managed by the Network Manager will be Disabled\nin '+delay+' seconds\n\nYou can click The Applet (NOT this OK button) to abort disabling the network" --timeout='+delay);
            this.playAlert();
            this.abortFlag = false;
            Mainloop.timeout_add_seconds(delay, Lang.bind(this, this.networkingDisable));

    },

     networkingDisable: function () {
          if(!this.abortFlag) {
               this._client.networking_enabled = false; // Call to NMClient
               alertModalNetworkingDisabled = new AlertDialog("THE DATA USAGE LIMIT HAS BEEN EXCEEDED\n\nAll Network connections managed by the Network Manager have been Disabled \n\nYou will need to use the Network Manager Applet to Re-enable them\nwhen the data usage problem has been resolved \n\n Some connections using ppp0 may not have been disabled\nor may need to be restarted manually");
              alertModalNetworkingDisabled.open();
          } else {
                GLib.spawn_command_line_async('zenity --info --text="You have aborted network disconnection despite the data usage limit being exceeded" --timeout=30');    
          }
      },


    // Called when the total limit has just been exceeded. 
    crisis: function () {

        if (this.crisisManagement == "notify") {
            // built in notify does not seem to work so use command line call
            // this.notify("The Limit has been exceeded", "Reset or turn off networking");
            GLib.spawn_command_line_async('notify-send "Current Network Usage Limit Exceeded" "You need to change the Data Limit for the current connection\n    Right Click -> Settings and adjust the Spin Wheel \n\n or disconnect using the Network Manager Applet" --urgency=critical');

        } else if (this.crisisManagement == "alertmodal") {
            alertModalDataWarning = new AlertDialog("THE DATA USAGE LIMIT HAS BEEN EXCEEDED\n\n Either set a new limit using right click -> Settings \n\nor disconnect using the Network Manager Applet");
            alertModalDataWarning.open();

        } else if (this.crisisManagement == "alertscript") {
            GLib.spawn_command_line_async('sh ' + this.appletPath + '/alertScript');

        } else if (this.crisisManagement == "suspendscript") {
            GLib.spawn_command_line_async('sh ' + this.appletPath + '/suspendScript');

        } else if (this.crisisManagement == "disablenetworking") {
              this.delayedNetworkingDisable(this.disconnectDelay);

        } else {
            GLib.spawn_command_line_async('notify-send "Current Network Usage Limit exceeded"  "You need to change the Data Limit for the current connection\n    Right Click -> Settings and adjust the Spin Wheel \n\n or disconnect using the Network Manager Applet" ');
        }
    },

    // This is the main update run in a loop with a timer 
    update: function () {
        if (this.monitoredInterfaceName != null) {
            let timeNow = GLib.get_monotonic_time();
            let deltaTime = (timeNow - this.timeOld) / 1000000;
            GTop.glibtop_get_netload(this.gtop, this.monitoredInterfaceName);
            let upNow = this.gtop.bytes_out;
            let downNow = this.gtop.bytes_in;
            if (deltaTime != 0) {
                if (!this.compactDisplay) {
                    this.labelOne.set_text("\u2193 " + this.formatSpeed((downNow - this.downOld) / deltaTime));
                    this.labelTwo.set_text("\u2191 " + this.formatSpeed((upNow - this.upOld) / deltaTime));
                } else {
                    this.labelOne.set_text("\u21f5 " + this.formatSpeed((upNow - this.upOld + downNow - this.downOld) / deltaTime));
                    this.labelTwo.set_text("");
                }
            }

            // Now check if we have just exceeded the total limit and need crisis action
            if ((upNow + downNow) / 1048576 > this.totalLimit && this.useTotalLimit && this.useCrisisManagement && (this.upOld + this.downOld) / 1048576 < this.totalLimit) {
                this.crisis();
            }

            // Update Old values
            this.upOld = upNow;
            this.downOld = downNow;
            this.timeOld = timeNow;
        } else {
            this.labelDownload.set_text("No Interface Set!");
        }
        // Update the three sets of cumulative usage data - Note this uses feature that gTop... delivers 0 if Interface does not exist or is inactive
        GTop.glibtop_get_netload(this.gtop, this.cumulativeInterface1);
        let upNowC1 = this.gtop.bytes_out;
        let downNowC1 = this.gtop.bytes_in;
        if (((downNowC1 > this.downOldC1) || (upNowC1 > this.upOldC1))) {
            this.cumulativeTotal1 = this.cumulativeTotal1 + (downNowC1 - this.downOldC1 + upNowC1 - this.upOldC1) / 1048576;
        }
        this.upOldC1 = upNowC1;
        this.downOldC1 = downNowC1;

        GTop.glibtop_get_netload(this.gtop, this.cumulativeInterface2);
        let upNowC2 = this.gtop.bytes_out;
        let downNowC2 = this.gtop.bytes_in;
        if (((downNowC2 > this.downOldC2) || (upNowC2 > this.upOldC2))) {
            this.cumulativeTotal2 = this.cumulativeTotal2 + (downNowC2 - this.downOldC2 + upNowC2 - this.upOldC2) / 1048576;
        }
        this.upOldC2 = upNowC2;
        this.downOldC2 = downNowC2;

        GTop.glibtop_get_netload(this.gtop, this.cumulativeInterface3);
        let upNowC3 = this.gtop.bytes_out;
        let downNowC3 = this.gtop.bytes_in;
        if (((downNowC3 > this.downOldC3) || (upNowC3 > this.upOldC3))) {
            this.cumulativeTotal3 = this.cumulativeTotal3 + (downNowC3 - this.downOldC3 + upNowC3 - this.upOldC3) / 1048576;
        }
        this.upOldC3 = upNowC3;
        this.downOldC3 = downNowC3;

        // Now set up tooltip every cycle
        if (this.monitoredInterfaceName != null) {
            this.set_applet_tooltip("Interface: " + this.monitoredInterfaceName + " - Downloaded: " + this.formatSentReceived(this.downOld) + " - Uploaded: " + this.formatSentReceived(this.upOld));
        }
        // Update selected items in left click menu every cycle
        this.menuitemInfo.label.text = "    " + this.monitoredInterfaceName + " - Downloaded: " + this.formatSentReceived(this.downOld) + " - Uploaded: " + this.formatSentReceived(this.upOld);

        //  rebuild Right Click menu but only when required and after changes flagged as it is a slow activity
        if (this.rebuildFlag) {
            this.rebuildFlag = false;
            this.buildContextMenu();
        }
        // Set background colour - green when data has flowed, orange when alert level reached and red when data limit reached
 //       this.actor.style = "background-color: rgba(0,0,0,0.0); border-radius: 10px" + "; width:" + this.appletWidth + "px"
       this.numa_style = 'numa-not-not-connected';
        if ((this.downOld + this.upOld) > 0) {
//           this.actor.style = "background-color: rgba(0,255,0,0.3); border-radius: 10px" + "; width:" + this.appletWidth + "px";
           this.numa_style = 'numa-connected';
        }
        if (this.useTotalLimit) {
            if ((this.downOld + this.upOld) / 1048576 > this.totalLimit * this.alertPercentage / 100) {
 //               this.actor.style = "background-color: orange; border-radius: 10px" + "; width:" + this.appletWidth + "px";
                this.numa_style = 'numa-alert';
            }
            if ((this.downOld + this.upOld) / 1048576 > this.totalLimit) {
//                this.actor.style = "background-color: red; border-radius: 10px" + "; width:" + this.appletWidth + "px";
                this.numa_style = 'numa-limit-exceeded'
            }
        }

        // Now set the style for the Applet using styles from a stylesheet.csswidth and width from Settings
        this.actor.remove_style_class_name(this.last_numa_style); // We need to clear previous style
        this.actor.add_style_class_name(this.numa_style); // Now add the new style
        this.actor.set_style('width:' + this.appletWidth + 'px')
        this.last_numa_style = this.numa_style;

        // Loop update
        let timer = this.refreshIntervalIn * 1000;
        Mainloop.timeout_add((timer), Lang.bind(this, this.update));
    },

    formatSpeed: function (value) {
        let decimalAdjust = Math.pow(10, (this.decimalsToShowIn));
        if (value < 1048576) return (Math.floor(value / 1024 * decimalAdjust) / decimalAdjust) + " KB/s";
        else return (Math.floor(value / 1048576 * decimalAdjust) / decimalAdjust) + " MB/s";
    },

    formatSentReceived: function (value) {
        if (value < 1048576) return Math.floor(value / 1024) + " KB";
        else return Math.floor((value / 1048576) * 10) / 10 + " MB";
    },

    on_applet_removed_from_panel: function () {
        this.settings.finalize();
    }
};

function main(metadata, orientation, panel_height, instance_id) {
    let myApplet = new MyApplet(metadata, orientation, panel_height, instance_id);
    return myApplet;
}
/* 
Version v18_2.3.7
1.0 Applet Settings now used for Update Rate, Resolution and Interface. 
    Built in function used for left click menu. 
1.1 Right click menu item added to open Settings Screen. 
1.2 Default connection added. Warning on data limit exceeded added. 
1.3 Slider and information lines added to left click menu to set Alarm level. 
1.4 Three cumulative data monitors added. 
1.5 Check for changed devices added to Right Click menu. 
1.6  Alert display slider etc inhibited when inactive. 
1.7 Add const UUID, Rebuild left on various settings changes.
1.7.1 Errors corrected
1.7.2 Beautified at http://jsbeautifier.org/ 
1.8.0 Added green background when data has flowed on selected interface.
1.8.1 Added more comments
1.9.0 Cumulative Usage now independent of interface being monitored
1.9.1 Indentation corrections
1.9.2 Reordering to make more clear
1.9.3 Beautify to spaces not tabs
1.10.0 Corrected error in handling tooltip at start-up  - V18_1 - as uploaded
2.0.0 New version with addition of vnstat statistics
2.0.1 Minor label and layout changes 03-07-2013
2.0.2 mainBox added so I can use remove_actor to avoid the need for restart when vnstati is not used. 
2.0.3 Error checking for installation of vnstat and vnstati and change of location of temp file to applet folder in applet
2.0.4 U: and D: replaced by unicode up and down arrows to save space.
2.0.5 New option of Compact Display of total only rather than Up and Down loads
2.0.6 Test version - needs sagarmatha restart
2.0.7 Compact Display and Width set in settings and implemented fully without restart required.
2.0.8 Added access to stylesheet.css and changelog.txt 09-07-2013
2.0.9 Compact Display can be changed in Context menu 10-07-2013 
2.1.0 Uses tick \u2714 for selected interface - UPLOADED as development version on 11-07-2013
2.1.1 Picks up UUID from metadata.uuid
2.1.2 Puts Housekeeping and System items in a sub-menu 13-07-2013
2.1.3 Minor correction in initialisation of cumulative data and layout/comments improved 14-07-2013
2.2.0 Start of implementing some form of response to exceeding data limits - termed crisis management
2.2.1 Crisis Management flag and 'terminal command' to be run now in Settings Panel.
2.2.2 Facility for two preset scripts in addition to string added. Right click menu has additional housekeeping functions to edit and test these scripts added.  
2.2.3 Addition documentation
2.2.4 Changed 'crisis' to 'suspend' for preset script and tested. Reorder advanced functions and added separators.
2.2.5 Three special cases now in use - suspendscript, alertscript and modalalert. Function added to implement the modal dialog used by modal alert. Testing functions still 'exposed' to users on right click function submenu. 
To think about - do we need the ability to have a terminal command option as any terminal command can be put into the alertScript file, in fact it can be a series of commands run asyncronously by ending them with a $ so a sound file could be played and a notification put up using zenity at the same time. This is a safer way ahead whilst leaving a huge flexibility for customisation. Should a different selection mechanism be used? 
Conclusion - change to a drop down selection of options, initially the three currently in use but consider adding sound and notification options. remove terminal string option as it can be in a script file.
2.2.6 Implemented drop down alert handling plus change back to KB from kB and replace round with floor
2.2.7 Scripts commented and checked. Extra options of notify and do nothing added. Use now made of Sox to play audio warnings and notify-send to add notifications. sox needs to be installed
2.2.8 Beautified at http://jsbeautifier.org/
2.2.9 Help changed to open http:www.pcurtis.com/summit.htm#numa until I get time to write a help file - UPLOADED TO WEB SITE on 08-08-2013
2.3.0 Modified call to modalDialog to have parameter with the text string to display
      Added option to Disable Networking using NMClient (Does not disable ppp0 connections not initiated by network manager)
2.3.1 Adjustable Delay time when disabling added to Settings
      Various bits of code tidied up
2.3.2 Added extra Settings - displayExtraHousekeeping, useAlertSound and alertSound (sound file to play)
      and code to implement
2.3.3 Quick fix to background colours to use transparency so they work with light and dark themes 02-09-2013
2.3.4 Addition of radiused borders to background to improve appearance
2.3.5 Major change in use of css styles for the background colours which show connection and alert status.
      This allows the user to match colours etc to a particular theme.
2.3.6 Minor bug fix - Context Menu not always rebuilt after adding or removing advanced functions submenu.
2.3.7 Bug fix - Cummulative counters 1 and 3 not being saved correctly 
*/
