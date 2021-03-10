Pebble.addEventListener('showConfiguration', function () {
    var config = JSON.parse(localStorage.getItem('config'));

    var url = 'https://aviinl.github.io/dict-config/oharean.html?' + encodeURIComponent(JSON.stringify(config));

    Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function (e) {
    // Decode the user's preferences
    var configData = JSON.parse(decodeURIComponent(e.response));
    localStorage.setItem('config', JSON.stringify(configData));
    sendConfig();
});

Pebble.addEventListener('ready', function () {
    sendConfig();
});


function sendConfig() {
    var timezone = 1;

    if(localStorage.getItem('config')) {
        var config = JSON.parse(localStorage.getItem('config'));

        if(config !== undefined && config.hasOwnProperty('timezone')) {
            timezone = parseInt(config.timezone);
        }
    }

    var dict = {
        'TimeZone': timezone
    };

    Pebble.sendAppMessage(dict, function () {
        console.log('Config data sent successfully!');
    }, function (e) {
        console.log('Error sending config data!');
    });
}