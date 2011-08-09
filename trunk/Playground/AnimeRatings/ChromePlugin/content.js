try {

var divs = document.getElementsByTagName("div");
for (var i = 0; i < divs.length; ++i) {
    if (divs[i].id == "mw-pages") {
        var lis = divs[i].getElementsByTagName("li");
        for (var j = 0; j < lis.length; ++j) {
            var link = lis[j].getElementsByTagName("a")[0];
            if (j == 6) {
                alert(link.title);
                chrome.extension.sendRequest({title: link.title}, function(response) {
                    alert(response);
                });
                break;
            }
        }
    }
}


chrome.extension.onRequest.addListener( function(request) {
    alert(JSON.stringify(request));
});

} catch(exc) {
    alert(JSON.stringify(exc));
}
