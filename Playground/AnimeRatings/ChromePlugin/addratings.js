
function getMALScore(callback, titleNode) {
  var xmlhttp = new window.XMLHttpRequest();
  xmlhttp.onreadystatechange = function(data) {
    if (xmlhttp.readyState == 4) {
      if (xmlhttp.status == 200) {
        callback(xmlhttp.responseText, titleNode);
      } else {
        callback(null, titleNode);
      }
    }
  }
  var url = "http://myanimelist.net/api/anime/search.xml?q=" + titleNode.innerText;
  xmlhttp.open('GET', url, true);
  xmlhttp.send();
}


function parseXML(xml, titleNode) {
    var parser = new DOMParser();
    var doc = parser.parseFromString(xml, "text/xml");
    var scores = doc.getElementsByTagName("score");
    if (scores.length > 0) {
        var score = scores[0];
        var actualScore = score.childNodes[0].nodeValue;
        titleNode.innerText = titleNode.innerText + " " + actualScore;
    }
}


function findTitles() {
    var titles = document.getElementsByTagName("a");
    for (var i = 0; i < titles.length; ++i) {
        var titleNode = titles[i];
        if (titleNode.innerText) {
            getMALScore(parseXML, titleNode);
        }
    }
}
findTitles();
