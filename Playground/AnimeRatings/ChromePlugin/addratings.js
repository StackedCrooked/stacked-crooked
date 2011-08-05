alert("1");
var http = new window.XMLHttpRequest();
alert("2");
http.onreadystatechange = function(data) {
  if (http.readyState == 4) {
    alert(JSON.stringify(http));
  }
}
alert("3");
var url = "http://myanimelist.net/api/anime/search.xml?q=Madoka";
alert("4");
http.open('GET', url, true);
alert("5");
http.send();
alert("6");
