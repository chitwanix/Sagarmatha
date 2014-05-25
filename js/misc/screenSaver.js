// -*- mode: js; js-indent-level: 4; indent-tabs-mode: nil -*-

const DBus = imports.dbus;
const Lang = imports.lang;

const ScreenSaverIface = {
    name: 'org.sagarmatha.ScreenSaver',
    methods: [{ name: 'GetActive',
                inSignature: '',
                outSignature: 'b' },
              { name: 'Lock',
                inSignature: '' },
              { name: 'SetActive',
                inSignature: 'b' }],
    signals: [{ name: 'ActiveChanged',
                inSignature: 'b' }]
};

function ScreenSaverProxy() {
    this._init();
}

ScreenSaverProxy.prototype = {
    _init: function() {
        DBus.session.proxifyObject(this,
                                   'org.sagarmatha.ScreenSaver',
                                   '/org/sagarmatha/ScreenSaver');

        DBus.session.watch_name('org.sagarmatha.ScreenSaver',
                                false, // do not launch a name-owner if none exists
                                Lang.bind(this, this._onSSAppeared),
                                Lang.bind(this, this._onSSVanished));

        this.screenSaverActive = false;
        this.connect('ActiveChanged',
                     Lang.bind(this, this._onActiveChanged));
    },

    _onSSAppeared: function(owner) {
        this.GetActiveRemote(Lang.bind(this, function(isActive) {
            this.screenSaverActive = isActive;
        }))
    },

    _onSSVanished: function(oldOwner) {
        this.screenSaverActive = false;
    },

    _onActiveChanged: function(object, isActive) {
        this.screenSaverActive = isActive;
    }
};
DBus.proxifyPrototype(ScreenSaverProxy.prototype, ScreenSaverIface);
