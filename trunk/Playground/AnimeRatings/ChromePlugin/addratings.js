alert("cookies");
chrome.cookies.getAll({}, function(cookies) { alert("Yes cookies"); });
alert("after cookies.");

