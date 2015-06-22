Pebble.addEventListener("appmessage", function(e) {
    var zodiacSign = e.payload.zodiacSign;
    Pebble.timelineSubscribe(
        zodiacSign,
        function () { 
            console.log('Subscribed to ' + zodiacSign);
        }, 
        function (errorString) { 
            console.log('Error subscribing to topic ' + zodiacSign + ': ' + errorString);
        }
    );
});