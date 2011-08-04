
function getMALScore(callback, title) {
  var xhr = new XMLHttpRequest();
  xhr.onreadystatechange = function(data) {
    if (xhr.readyState == 4) {
      if (xhr.status == 200) {
        callback(xhr.responseText);
      } else {
        callback(null);
      }
    }
  }
  // Note that any URL fetched here must be matched by a permission in
  // the manifest.json file!
  var url = "http://myanimelist.net/api/anime/search.xml?q=" + title;
  xhr.open('GET', url, true);
  xhr.send();
}


function findTitles() {
    var titles = document.getElementsByTagName("a");
    for (var i = 40; i < titles.length; ++i) {
        var title = titles[i];
        if (title.innerText) {
            getMALScore(function(xml) {
                alert(xml);
            }, title.innerText);
        }
        break;
    }
}
findTitles();
