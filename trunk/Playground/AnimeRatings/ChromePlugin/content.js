function AnimeRatings() {

	/// Redefine alert
	this._debug = true;
	this.alert = function(msg) {
		if (!confirm(msg + '\n\nPress Cancel to stop debugging.')) {
			this._debug = false;
		}
	};
	window.alert = this.alert;


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


var ar = new AnimeRatings();

ar.getMWPages = function() {
	var divs = document.getElementsByTagName("div");
	for (var i = 0; i < divs.length; ++i) {
		if (divs[i].id == "mw-pages") {
			return divs[i];
		}
	}
	throw "Could not find root node for the anime titles.";
};

ar.getLinksImpl = function(mwpages) {
	var result = [];
	var lis = mwpages.getElementsByTagName("li");
	for (var i = 0; i < lis.length; ++i) {
		var links = lis[i].getElementsByTagName("a");
		if (links.length > 0) {
			result.push(links[0]);
		}
	}
	if (result.length === 0) {
		throw "No page links were found.";
	}
	return result;
};


ar.addToDOM = function(linkItem) {
	assertProperty(linkItem, "node");
	assertProperty(linkItem, "entries");

	var node = linkItem.node;
	var entries = linkItem.entries;
	var parent = node.parentNode;

	var ul = document.createElement("ul");
	parent.appendChild(ul);
	parent = ul;

	var small = document.createElement("small");
	parent.appendChild(small);
	parent = small;

	for (var i = 0; i < entries.length; ++i) {

		var entry = entries[i];

		assertProperty(entry, "score");
		assertProperty(entry, "url");

		// Score 0.00 means that there are not enough votes
		// to determine a weighted score. These results are
		// not interesting for our application.
		if (entry.score === "0.00") {
			continue;
		}

		var li = document.createElement("li");
		parent.appendChild(li);
		parent = li;

		var oldParent = parent;
		if (parseInt(entry.score,10) >= 8) {
			var bold = document.createElement("b");
			parent.appendChild(bold);
			parent = bold;
		}

		parent.appendChild(document.createTextNode("\u00a0\u00a0"));

		var malLink = document.createElement("a");
		malLink.setAttribute("href", entry.url);
		parent.appendChild(malLink);

		var entryText = entry.title + " (" + entry.score + ")";
		var malScoreText = document.createTextNode(entryText);
		malLink.appendChild(malScoreText);

		parent = oldParent;
	}
};


ar.getMALInfo = function(title, callback) {
	var linkInfo = {};
	linkInfo.title = title;
	this.sendRequest(
		{action: "getMalInfo", arg: linkInfo},
		function(linkInfo) {
			assert(linkInfo);
			assert(linkInfo.success);
			if (linkInfo.success === true) {
				assertProperty(linkInfo, "entries");
				callback(linkInfo);
			}
		}
	);
};


ar.getLinks = function(callback) {
	var linkNodes = this.getLinksImpl(this.getMWPages());
	for (var i = 0; i < linkNodes.length; ++i) {
		var linkNode = linkNodes[i];
		callback(linkNode);
	}
};


//
// Application Entry Point
//
ar.getLinks(function(linkNode) {
	ar.getMALInfo(linkNode.title, function(linkInfo) {
		linkInfo.node = linkNode;
		ar.addToDOM(linkInfo);
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

