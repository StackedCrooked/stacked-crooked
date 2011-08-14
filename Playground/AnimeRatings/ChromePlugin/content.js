function AnimeRatings() {

}


try {


var animeRatings = new AnimeRatings();


animeRatings.sendRequest = function(arg, callback) {
	chrome.extension.sendRequest(arg, function(response) {
		callback(response);
	});
};


animeRatings.getMWPages = function() {
	var divs = document.getElementsByTagName("div");
	for (var i = 0; i < divs.length; ++i) {
		if (divs[i].id == "mw-pages") {
			return divs[i];
		}
	}
	throw "MW Pages not found";
};


animeRatings.getLinksImpl = function() {
	var result = [];
	try {
		var lis = this.getMWPages().getElementsByTagName("li");
		for (var i = 0; i < lis.length; ++i) {
			var links = lis[i].getElementsByTagName("a");
			if (links.length > 0) {
				result.push(links[0]);
			}
		}
	}
	catch (exc) {
		// MW pages not found on this page.
		// This can happen for summary pages like "Category:Anime_of_the_2000s"
	}
	return result;
};


animeRatings.addToDOM = function(linkItem) {
	var node = linkItem.node;
	var parent = node.parentNode;

	var entries = linkItem.entries;
	entries.sort(function(lhs, rhs) {
		if (lhs.start_date < rhs.start_date) {
			return -1;
		}
		else if (lhs.start_date == rhs.start_date) {
			return 0;
		}
		return 1;
	});

	if (parent.getElementsByTagName("ul").length === 0) {
		var ul = document.createElement("ul");
		parent.appendChild(ul);
		parent = ul;
	}

	var oldParent = parent;
	for (var i = 0; i < entries.length; ++i) {

		var entry = entries[i];

		// Score 0.00 means that there are not enough votes
		// to determine a weighted score.
		if (entry.score === "0.00") {
			entry.score = "?";
		}

		var li = document.createElement("li");
		parent.appendChild(li);
		parent = li;

		var small = document.createElement("small");
		parent.appendChild(small);
		parent = small;

		var malLink = document.createElement("a");
		malLink.setAttribute("href", "http://myanimelist.net/anime/" + entry.id);
		parent.appendChild(malLink);
		parent = malLink;


		if (parseFloat(entry.score,10) >= 7) {
			var bold = document.createElement("b");
			parent.appendChild(bold);
			parent = bold;

			var span = document.createElement("span");
			if (parseFloat(entry.score, 10) >= 9) {
				span.setAttribute("style", "color: red;");
			}
			else if (parseFloat(entry.score, 10) >= 8) {
				span.setAttribute("style", "color: orange;");
			}
			else {
				span.setAttribute("style", "color: green;");
			}
			parent.appendChild(span);
			parent = span;
		}

		var entryText = entry.start_date.split("-")[0] + " " + entry.title + " (" + entry.score + ")";
		var malScoreText = document.createTextNode(entryText);
		parent.appendChild(malScoreText);

		parent = oldParent;
	}
};


animeRatings.getMALInfo = function(title, callback) {
	var linkInfo = {};
	linkInfo.title = title;
	this.sendRequest(
		{action: "getMalInfo", arg: linkInfo},
		function(linkInfo) {
			callback(linkInfo);
		}
	);
};


animeRatings.getLinks = function(callback) {
	var linkNodes = this.getLinksImpl();
	for (var i = 0; i < linkNodes.length; ++i) {
		var linkNode = linkNodes[i];
		callback(linkNode);
	}
};


animeRatings.improveTitle = function(title) {
	var mapping = {
		"A Channel": "A-Channel",
		"×" : "x",
		"ō" : "ou",
		"ä" : "a",
		"½" : "1/2",
		" (manga)": "",
		" (anime)": ""
	};

	for (var key in mapping) {
		var value = mapping[key];
		title = title.replace(key, value);
	}

	return title;
};


//
// Application Entry Point
//
animeRatings.getLinks(function(linkNode) {
	var title = animeRatings.improveTitle(linkNode.title);

	animeRatings.getMALInfo(title, function(linkInfo) {
		linkInfo.node = linkNode;
		if (linkInfo.success === true) {
			animeRatings.addToDOM(linkInfo);
		}
	});
});


} catch (exc) {
	if (typeof(exc) === "string") {
		alert(exc);
	}
	else {
		alert(JSON.stringify(exc));
	}
}

