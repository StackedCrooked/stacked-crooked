function AnimeRatings() {


	this.onResponse = function(response) {
		alert("this.onResponse: " + response);
	};

	this.sendRequest = function(arg) {
		chrome.extension.sendRequest(arg, this.onResponse);
	};

	this.sendRequest("test");

//	this.getMWPages = function() {
//		var divs = document.getElementsByTagName("div");
//		for (var i = 0; i < divs.length; ++i) {
//			if (divs[i].id == "mw-pages") {
//				return divs[i];
//			}
//		}
//		throw "Could not find root node for the anime titles.";
//	};

//	this.getLinks = function(mwpages) {
//		var result = [];
//		var lis = mwpages.getElementsByTagName("li");
//		for (var i = 0; i < lis.length; ++i) {
//			var links = lis[i].getElementsByTagName("a");
//			if (links.length > 0) {
//				result.push(links[0]);
//			}
//		}
//		if (result.length === 0) {
//			throw "No page links were found.";
//		}
//		return result;
//	};

//	this.onRequest = function(request) {
//		throw request;
//	};

//	this.log = function(obj) {
//		this.sendRequest({ action: "log", arg: obj }, this.onResponse);
//	};

//	// Register the request handler
//	chrome.extension.onRequest.addListener(this.onRequest);

//	var links = this.getLinks(this.getMWPages());
//	for (var i = 0; i < links.length; ++i) {
//		console.log(links[i].title);
//	}
}

try {
	var ar = new AnimeRatings();
}
catch (exc) {
	alert(JSON.stringify(exc));
}

