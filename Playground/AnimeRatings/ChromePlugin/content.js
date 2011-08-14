function AnimeRatings() {
}


try {


var animeRatings = new AnimeRatings();


animeRatings.sendRequest = function(arg, callback) {
	chrome.extension.sendRequest(arg, function(response) {
		callback(response);
	});
};


animeRatings.log = function(message) {
	this.sendRequest(
		{action: "log", arg: message},
		function() {}
	);
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


animeRatings.getMWPages = function() {
	var divs = document.getElementsByTagName("div");
	for (var i = 0; i < divs.length; ++i) {
		if (divs[i].id == "mw-pages") {
			return divs[i];
		}
	}
	throw "MW Pages not found";
};


animeRatings.getLinks = function() {
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


animeRatings.decorate = function(parent, name) {
	var el = document.createElement(name);
	parent.appendChild(el);
	return el;
};


animeRatings.addEntryToDOM = function(parent, entry) {

	// Score 0.00 means that there are not enough votes
	// to determine a weighted score.
	if (entry.score === "0.00") {
		entry.score = "?";
	}

	parent = animeRatings.decorate(parent, "li");

	parent = animeRatings.decorate(parent, "a");
	parent.setAttribute("href", "http://myanimelist.net/anime/" + entry.id);

	if (parseFloat(entry.score, 10) >= 8) {
		parent.setAttribute("style", "background-color:yellow;");
		parent = animeRatings.decorate(parent, "strong");
	}

	var entryText = entry.start_date.split("-")[0] + " " + entry.title + " (" + entry.score + ")";
	parent.appendChild(document.createTextNode(entryText));
};


animeRatings.informFailure = function(linkItem) {
	var node = linkItem.node;
	var parent = node.parentNode;

	if (parent.getElementsByTagName("ul").length === 0) {
		parent = this.decorate(parent, "small");
		parent = this.decorate(parent, "ul");
	}
	parent = animeRatings.decorate(parent, "li");
	parent.appendChild(document.createTextNode("Not found"));
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
		parent = this.decorate(parent, "small");
		parent = this.decorate(parent, "ul");
	}

	var oldParent = parent;
	for (var i = 0; i < entries.length; ++i) {
		try {
			var entry = entries[i];
			this.addEntryToDOM(parent, entry);
		}
		catch (exc) {
			console.log(exc);
		}
		parent = oldParent;
	}
};


/**
 * Workaround: improves search results
 */
animeRatings.improveTitle = function(title) {

	var result = title;

	// If the title contains a fragment, then only search for the mapped fragment.
	var tmapping = {
		"A Channel": "A-Channel",
		"Ano Hi Mita Hana": "Ano Hi Mita Hana",
		"Doraemon" : "Doraemon",
		"Infinite Stratos" : "Infinite Stratos",
		"Maji de Watashi ni Koishinasai!" : "Maji de Watashi ni Koi Shinasai!",
		"Mawaru-Penguindrum" : "Mawaru Penguindrum",
		"Poppy Hill" : "Kokurikozaka Kara",
		"Heaven's Lost Property" : "Heaven's Lost Property"
	};

	for (var tkey in tmapping) {
		if (result.search(tkey) !== -1) {
			result = tmapping[tkey];
			break;
		}
	}

	// Improve fragments
	var fmapping = {
		"×" : "x",
		"ō" : "ou",
		"ū" : "uu",
		"ä" : "a",
		"½" : "1/2",
		"(anime)": "",
		"(film)": "",
		"(manga)": "",
		"(movie)": "",
		"(video game)": "",
		"(novel series)" : ""
	};

	for (var fkey in fmapping) {
		var fragment = fmapping[fkey];
		var count = 0;
		while (result.search(fkey) !== -1 && count < 10) {
			result = result.replace(fkey, fragment);
			count++;
		}
		if (count >= 10) {
			this.log("Problematic replacement: " + result + ", with key: " + fkey);
		}
	}

	// Trim
	result = result.replace(/^\s+|\s+$/g, "");

	return result;
};


animeRatings.getNext = function(links) {
	if (links.length === 0) {
		return;
	}

	var linkNode = links.pop();
	var title = animeRatings.improveTitle(linkNode.title);
	this.getMALInfo(title, function(linkInfo) {
		linkInfo.node = linkNode;
		if (linkInfo.success === true) {
			animeRatings.addToDOM(linkInfo);
		}
		else {
			animeRatings.informFailure(linkInfo);
		}
		animeRatings.getNext(links);
	});
};


//
// Application Entry Point
//
var links = animeRatings.getLinks().reverse();
animeRatings.getNext(links);


} catch (exc) {
	animeRatings.log(exc);
}
