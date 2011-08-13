function AnimeRatings() {

	this.sendRequest = function(arg, callback) {

		chrome.extension.sendRequest(arg, function(response) {
			assertProperty(response, "success");
			callback(response);
		});
	};


	this.log = function(message) {
		this.sendRequest({ action: "log", arg: message }, this.onResponse);
	};
	window.log = this.log;

}


function assertProperty(obj, prop) {
	if (obj[prop] === undefined) {
		throw arguments.callee.caller.toString() + "\n\nMissing property: '" + prop + "'.";
	}
}


function assert(obj) {
	if (obj === undefined || obj === null || obj === false) {
		throw "Failed assert: " + arguments.callee.caller.toString();
	}
}


try {


var animeRatings = new AnimeRatings();

animeRatings.getMWPages = function() {
	var divs = document.getElementsByTagName("div");
	for (var i = 0; i < divs.length; ++i) {
		if (divs[i].id == "mw-pages") {
			return divs[i];
		}
	}
	return null;
};

animeRatings.getLinksImpl = function(mwpages) {
	var result = [];
	var lis = mwpages.getElementsByTagName("li");
	for (var i = 0; i < lis.length; ++i) {
		var links = lis[i].getElementsByTagName("a");
		if (links.length > 0) {
			result.push(links[0]);
		}
	}
	return result;
};


animeRatings.addToDOM = function(linkItem) {
	var node = linkItem.node;
	var parent = node.parentNode;


	if (linkItem.success === false) {
		parent.appendChild(document.createTextNode(linkItem.reason));
	}

	var entries = linkItem.entries;

	if (parent.getElementsByTagName("ul").length === 0) {
		var ul = document.createElement("ul");
		parent.appendChild(ul);
		parent = ul;
	}

	var oldParent = parent;

	entries.sort(function(lhs, rhs) {
		if (lhs.start_date < rhs.start_date) {
			return -1;
		}
		else if (lhs.start_date == rhs.start_date) {
			return 0;
		}
		return 1;
	});
	for (var i = 0; i < entries.length; ++i) {

		var entry = entries[i];

		// Score 0.00 means that there are not enough votes
		// to determine a weighted score. These results are
		// not interesting for our application.
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


		if (parseInt(entry.score,10) >= 7) {
			var bold = document.createElement("b");
			parent.appendChild(bold);
			parent = bold;

			if (parseInt(entry.score, 10) >= 9) {
				var span = document.createElement("span");
				span.setAttribute("style", "color: red;");
				parent.appendChild(span);
				parent = span;
			}
			else if (parseInt(entry.score, 10) >= 8) {
				var span2 = document.createElement("span");
				span2.setAttribute("style", "color: green;");
				parent.appendChild(span2);
				parent = span2;
			}
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
			assert(linkInfo);
			assert(linkInfo.success);
			callback(linkInfo);
		}
	);
};


animeRatings.getLinks = function(callback) {
	var mwPages = this.getMWPages();
	if (mwPages === null) {
		return;
	}
	var linkNodes = this.getLinksImpl(mwPages);
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
		" (manga)": ""
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
		animeRatings.addToDOM(linkInfo);
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

