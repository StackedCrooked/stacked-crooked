
function getMALScore(callback, titleNode) {
  var xmlhttp = new window.XMLHttpRequest();
  xmlhttp.onreadystatechange = function(data) {
    if (xmlhttp.readyState == 4) {
      if (xmlhttp.status == 200) {
        callback(xmlhttp.responseText);
      } else {
        callback(null);
      }
    }
  }
  // Note that any URL fetched here must be matched by a permission in
  // the manifest.json file!
  var url = "http://myanimelist.net/api/anime/search.xml?q=" + titleNode.innerText;
  xmlhttp.open('GET', url, true);
  xmlhttp.send();
}


function parseXML(xml) {
    alert(xml);
    var parser = new DOMParser();
    alert("parser: " + parser);
    var doc = parser.parseFromString(xml, "text/xml");
    alert("doc: " + doc);
    var scores = doc.getElementsByTagName("score");
    alert("scores: " + scores);
    if (scores.length > 0) {
        var score = scores[0];
        alert("score: " + score);
        var actualScore = score.childNodes[0].nodeValue;
        alert("actualScore: " + actualScore);
    }
}


function findTitles() {
    var titles = document.getElementsByTagName("a");
    for (var i = 39; i < titles.length; ++i) {
        var titleNode = titles[i];
        if (titleNode.innerText) {
            getMALScore(parseXML, titleNode);
        }
        break;
    }
}
findTitles();
