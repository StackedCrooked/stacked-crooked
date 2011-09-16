function AnimeRatings() {
}

var animeRatings = new AnimeRatings();
animeRatings.cache = {};


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


animeRatings.getPageType = function() {
    if (document.URL.search("Category:Anime_of_") !== -1) {
        return "anime";
    }
    else if (document.URL.search("Category:Manga_of_") !== -1) {
        return "manga";
    }
    return "default";
};


animeRatings.getMALInfo = function(pageType, title, callback) {
    var linkInfo = {};
    linkInfo.title = title;
    linkInfo.pageType = pageType;
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


animeRatings.debugLink = "";
animeRatings.excludeLink = "";


animeRatings.getLinks = function() {
    var result = [];
    try {
        var lis = this.getMWPages().getElementsByTagName("li");
        for (var i = 0; i < lis.length; ++i) {
            var links = lis[i].getElementsByTagName("a");
            if (links.length > 0) {
                var linkNode = links[0];
                if (linkNode.title.search(animeRatings.debugLink) !== -1 &&
                    linkNode.title.search(animeRatings.excludeLink === -1)) {
                    result.push(linkNode);
                }
            }
        }
    }
    catch (exc) {
        // MW pages not found on this page.
        // This can happen for summary pages like "Category:Anime_of_the_2000s"
    }
    return result;
};


animeRatings.encodeResult = function(title) {

    var result = title;
    var keys = [];
    var values = [];
    keys.push(/&Atilde;&copy;/g); values.push("&eacute;");
    keys.push(/&Atilde;&uml;/g); values.push("&egrave;");
    keys.push(/&Atilde;&ordf;/g); values.push("&ecirc;");
    keys.push(/&Atilde;&laquo;/g); values.push("&euml;");
    keys.push(/&Atilde;&nbsp;/g); values.push("&agrave;");
    keys.push(/&Atilde;&curren;/g); values.push("&auml;");
    keys.push(/&Atilde;&cent;/g); values.push("&acirc;");
    keys.push(/&Atilde;&sup1;/g); values.push("&ugrave;");
    keys.push(/&Atilde;&raquo;/g); values.push("&ucirc;");
    keys.push(/&Atilde;&frac14;/g); values.push("&uuml;");
    keys.push(/&Atilde;&acute;/g); values.push("&ocirc;");
    keys.push(/&Atilde;&para;/g); values.push("&ouml;");
    keys.push(/&Atilde;&reg;/g); values.push("&icirc;");
    keys.push(/&Atilde;&macr;/g); values.push("&iuml;");
    keys.push(/&Atilde;&sect;/g); values.push("&ccedil;");
    keys.push(/&amp;/g); values.push("&amp;");

    keys.push(/&nbsp;/g); values.push(" ");
    keys.push(/&auml;/g); values.push("ä");
    keys.push(/&uuml;/g); values.push("ü");

    for (var i = 0; i < keys.length; ++i) {
        result = result.replace(keys[i], values[i]);
    }
    return result;
};


animeRatings.htmlDecode = function(input) {
    var e = document.createElement('div');
    e.innerHTML = input;
    return e.childNodes.length === 0 ? "" : e.childNodes[0].nodeValue;
};


animeRatings.findAndReplace = function(input, mapping) {
    var result = input;
    for (var key in mapping) {
        var value = mapping[key];
        var count = 0;
        while (result.search(key) !== -1) {
            result = result.replace(key, value);
            if (count++ > 20) {
                this.log("Problematic replacement key: " + key);
                break;
            }
        }
    }
    return result;
};

/**
 * For some reason the unicode in the xml response
 * is wrong. Either I am doing something wrong, or
 * MAL wrongly encodes the response.
 *
 * This code is a workaround that provides fixes
 * for common cases.
 */
animeRatings.fixUnicode = function(input) {
    var result = input;

    // First apply a mapping of composed keys
    result = this.findAndReplace(result, {
        "&acirc;��&acirc;��&acirc;��"	: "☆☆☆",
        "&Atilde;�&Atilde;�&Atilde;�"		:"xxx",
        "&acirc;��"						: "-"
    });

    // Then map remaining individual keys
    result = this.findAndReplace(result, {
        "&Atilde;&copy;"	: "é",
        "&acirc;�&ordf;"	: "♪",
        "&acirc;�"			: "†",
        "&Aring;�"			: "ō"
    });
    return result;
};


animeRatings.getYear = function() {
    var components = document.URL.split("_");
    return components[components.length - 1];
};


/**
 * @pattern  String  "{Year} {Title} {Score}"
 */
animeRatings.addEntryToDOM = function(node, entry, pattern) {

    var parent = node.create("a");
    parent.setAttribute("href", "http://myanimelist.net/" + this.getPageType() + "/" + entry.id);

    if (parseFloat(entry.score, 10) >= 8) {
        parent = parent.create("strong");
        parent.setAttribute("style", "background-color:yellow;");
    }

    var result = pattern;
    result = result.replace("{BeginYear}",  parseInt(entry.start_date.split("-")[0], 10));
    result = result.replace("{EndYear}",  parseInt(entry.end_date.split("-")[0], 10));
    result = result.replace("{Title}", entry.title);
    result = result.replace("{Score}", entry.score);
    parent.setInnerText(this.htmlDecode(this.fixUnicode(this.encodeResult(result))));
};


animeRatings.informFailure = function(node, linkItem) {
    var reason = (linkItem.reason === undefined ? "No results returned." : linkItem.reason);
    var parent = node.parentNode;
    parent = parent.createEntryList().create("li");
    parent.setInnerText(reason);
};


animeRatings.addEntriesToDOM = function(node, linkItem) {
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
        parent = parent.createEntryList();
        parent.style.listStyle = "square outside none";
    }

    for (var i = 0; i < entries.length; ++i) {
        try {
            var entry = entries[i];

            // Skip if year doesn't match
            var begin_year = parseInt(entry.start_date.split("-")[0], 10);
            var end_year = parseInt(entry.end_date.split("-")[0], 10);
            var year = this.getYear();
            if (begin_year > year || (end_year !== 0 && end_year < year)) {
                if (entry.title.search("Hunter") !== -1) {
                    this.log(year + " is outside of [" + begin_year + ", " + end_year + "]");
                    this.log("entry.end_date: " + entry.end_date);
                }
                parent.setAttribute("private_year_is_wrong", true);
                continue;
            }

            // Insert title entry
            this.addEntryToDOM(parent.create("li"),
                               entry,
                               "{Title} ({Score})");
        }
        catch (exc) {
            animeRatings.log(exc);
        }
    }

    if (parent.hasAttribute("private_year_is_wrong") &&
        parent.getElementsByTagName("li").length === 0) {
        animeRatings.addMissingStuff(parent, entries);
    }
};


animeRatings.addMissingStuff = function(listElement, entries) {

    if (entries.length === 0) {
        return;
    }

    listElement.style.listStyle = "none";

    var parent = listElement.create("li");
    parent.setInnerText("No MAL title found from " + this.getYear() + ". Closest match:");

    parent = parent.create("ul");
    parent.style.listStyle = "square outside none";

    var closest_entry = entries[0];
    closest_entry.difference = Math.abs(this.getYear() - parseInt(closest_entry.start_date.split("-")[0], 10));

    for (var i = 1; i < entries.length; ++i) {
        try {
            var entry = entries[i];
            entry.difference = Math.abs(this.getYear() - parseInt(entry.start_date.split("-")[0], 10));
            if (entry.difference < closest_entry.difference) {
                closest_entry = entry;
            }
        }
        catch (exc) {
            animeRatings.log(exc);
        }
    }

    // Insert title entry
    this.addEntryToDOM(parent.create("li"), closest_entry, "{Title} ({BeginYear}, {Score})");
};


/**
 * Workaround: improves search results
 */
animeRatings.improveTitle = function(title) {

    var result = title;

    // If the title contains a fragment, then only search for the mapped fragment.
    var tmapping = {
        "11eyes: Tsumi to Batsu to Aganai no Shōjo" : "11eyes",
        "A Channel": "A-Channel",
        "Aki Sora" : "Aki-Sora",
        "Ano Hi Mita Hana": "Ano Hi Mita Hana",
        "Doraemon" : "Doraemon",
        "Infinite Stratos" : "Infinite Stratos",
        "Mawaru-Penguindrum" : "Mawaru Penguindrum",
        "Poppy Hill" : "Kokurikozaka Kara",
        "Koishinasai" : "Koi Shinasai",
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
        "Ō" : "Ou",
        "ū" : "uu",
        "ä" : "a",
        "Ä" : "A",
        "½" : "1/2",
        "(anime)": "",
        "(film)": "",
        "(manga)": "",
        "(movie)": "",
        "(visual novel)" : "",
        "(novel)": "",
        "(video game)": "",
        "(novel series)" : "",
        "(Japanese series)" : "",
        "(TV series)" : ""
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


animeRatings.linkNodes = {};
animeRatings.links = animeRatings.getLinks().reverse();

Element.prototype.setInnerText = function(text) {
    this.appendChild(document.createTextNode(text));
};

Element.prototype.create = function(tagNamePath) {
    var result = this;
    var tagNames = tagNamePath.split("/");
    for (var i = 0; i < tagNames.length; ++i) {
        var child = document.createElement(tagNames[i]);
        result.appendChild(child);
        result = child;
    }
    return result;
};


Element.prototype.createEntryList = function() {
    var result = this.create("small/ul");
    result.style.listStyle = "square outside none";
    return result;
};

animeRatings.getNext = function() {
    if (animeRatings.links.length === 0) {
        return;
    }

    var linkNode = animeRatings.links.pop();

    var title = animeRatings.improveTitle(linkNode.title);
    if (animeRatings.linkNodes[title] === undefined) {
        animeRatings.linkNodes[title] = linkNode;
        this.getMALInfo(this.getPageType(), title, function(linkInfo) {
            var node = animeRatings.linkNodes[title];
            if (linkInfo.success === true) {
                animeRatings.addEntriesToDOM(node, linkInfo);
            }
            else {
                animeRatings.informFailure(node, linkInfo);
            }
            animeRatings.getNext();
        });
    }
};


animeRatings.isAnimeListing = function() {
    return animeRatings.getPageType() === "anime" ||
           animeRatings.getPageType() === "manga";
};


animeRatings.getANNLink = function() {
    var links = document.getElementsByTagName("a");
    for (var i = 0; i < links.length; ++i) {
        var link = links[i];
        var href = link.href;
        if (href.search("http://www.animenewsnetwork.com/encyclopedia") !== -1) {
            return link;
        }
    }
    return null;
};


animeRatings.getAnimeTitle = function() {
    var headings = document.getElementsByTagName("h1");
    if (headings.length === 0) {
        throw "H1 not found in page";
    }
    return headings[0].childNodes[0].innerHTML;
};


animeRatings.hasPageContainsAnimeInfoBox = function() {
    var tables = document.getElementsByTagName("table");
    for (var i = 0; i < tables.length; ++i) {
        var table = tables[i];
        if (table.getAttribute("class") === "infobox") {
            var tds = table.getElementsByTagName("td");
            for (var j = 0; j < tds.length; ++j) {
                var td = tds[j];
                if (td.childNodes.length === 0) {
                    continue;
                }

                if (td.childNodes[0].nodeValue === null) {
                    continue;
                }

                this.log("3 " + td.childNodes[0].nodeValue);
                if (td.childNodes[0].nodeValue.search(/anime/i) !== -1) {
                    return true;
                }
            }
        }
    }
    return false;
};


animeRatings.pageContainsLinkToAnimeNewsNetwork = function() {
    return this.getANNLink() !== null;
};


animeRatings.isAnimePage = function() {
    return !animeRatings.isAnimeListing() &&
        (animeRatings.pageContainsLinkToAnimeNewsNetwork() ||
         animeRatings.hasPageContainsAnimeInfoBox());

};


animeRatings.insertRatingsIntoYearList = function() {
    var numSimulReq = 5;
    for (var i = 0; i < numSimulReq; ++i) {
        animeRatings.getNext(animeRatings.links);
    }
};


animeRatings.insertRatingsIntoAnimePage = function() {
    var title = animeRatings.improveTitle(animeRatings.getAnimeTitle());
    this.getMALInfo("anime", title, function(linkInfo) {
        animeRatings.addRatingIntoAnimePageDOM(linkInfo);
    });
};


animeRatings.getFirstChildByTagName = function(node, tagName) {
    for (var i = 0; i < node.childNodes.length; ++i) {
        var childNode = node.childNodes[i];
        if (childNode.tagName === tagName) {
            return childNode;
        }
    }
    return null;
};


animeRatings.addRatingIntoAnimePageDOM = function(linkInfo) {

    var firstParagraph = animeRatings.getFirstChildByTagName(document.getElementById("bodyContent"), "P");

    var node = firstParagraph.parentNode.insertBefore(document.createElement("div"), firstParagraph);

    node = node.create("table");
    node.setAttribute("class", "toc");
    node = node.create("tr/td");
    node.create("h2").setInnerText("MyAnimeList matches");
    node = node.create("ul");

    for (var i = 0; i < linkInfo.entries.length; ++i) {

        var entry = linkInfo.entries[i];
        var a = node.create("li/a");
        a.setAttribute("href", "http://myanimelist.net/anime/" + entry.id);
        var result = "{Year}: {Title} ({Score})";
        var year = parseInt(entry.start_date.split("-")[0], 10);
        result = result.replace("{Year}", year !== 0 ? year : "????");
        result = result.replace("{Title}", entry.title);
        result = result.replace("{Score}", entry.score !== "0.00" ? entry.score : "?");
        a.setInnerText(this.htmlDecode(this.fixUnicode(this.encodeResult(result))));
    }
};


//
// Application Entry Point
//
try {

if (animeRatings.isAnimeListing()) {
    animeRatings.insertRatingsIntoYearList();
}
else if (animeRatings.isAnimePage()) {
    animeRatings.insertRatingsIntoAnimePage();
}

} catch (exc) {
    animeRatings.log("Exception caught: " + exc.toString());
}
