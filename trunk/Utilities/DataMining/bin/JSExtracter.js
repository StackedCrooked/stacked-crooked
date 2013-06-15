// Extract images from a webpage.
function extract(match, dispatch) {
    var images = document.getElementsByTagName('img');
    for (var i = 0; i != images.length; ++i) {
        var url = images[i].src;
        if (match(url)) {
            dispatch(url);
        }
    }
}

function test() {
    extract(
        function(url) {
            try {
                return -1 !== url.search(/\.jpg$/);
            } catch (e) {
                console.log("Error while parsing url. Reason: " + e);
            }
        },
        function(url) {
            console.log(url);
        });  
}

test();
