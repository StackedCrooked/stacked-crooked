#include "FacebookCpp/Method/users_getInfo.h"
#include <iostream>


using namespace FacebookCpp;
using namespace FacebookCpp::XSD;
using namespace Poco;
using namespace std;


users_getInfo::users_getInfo
(
	const Session & inSession,
	const vector<string> & inUids,
	const AbstractDelegate<ResponseType> & inSuccessCallback,
	const AbstractDelegate<ErrorResponse> & inErrorCallback
):
	FacebookSessionMethod(inSession, "users.getInfo", inErrorCallback),
	mSuccessCallback(inSuccessCallback)
{
	mParams["uids"] = toCSV(inUids);
	mParams["fields"] += "about_me";
	mParams["fields"] += ",activities";
	mParams["fields"] += ",affiliations";
	mParams["fields"] += ",birthday";
	mParams["fields"] += ",books";
	mParams["fields"] += ",current_location";
	mParams["fields"] += ",education_history";
	mParams["fields"] += ",first_name";
	mParams["fields"] += ",has_added_app";
	mParams["fields"] += ",hometown_location";
	mParams["fields"] += ",hs_info";
	mParams["fields"] += ",interests";
	mParams["fields"] += ",is_app_user";
	mParams["fields"] += ",last_name";
	mParams["fields"] += ",meeting_for";
	mParams["fields"] += ",meeting_sex";
	mParams["fields"] += ",movies";
	mParams["fields"] += ",music";
	mParams["fields"] += ",name";
	mParams["fields"] += ",notes_count";
	mParams["fields"] += ",pic";
	mParams["fields"] += ",pic_big";
	mParams["fields"] += ",pic_small";
	mParams["fields"] += ",pic_square";
	mParams["fields"] += ",political";
	mParams["fields"] += ",profile_update_time";
	mParams["fields"] += ",quotes";
	mParams["fields"] += ",relationship_status";
	mParams["fields"] += ",religion";
	mParams["fields"] += ",sex";
	mParams["fields"] += ",significant_other_id";
	mParams["fields"] += ",status";
	mParams["fields"] += ",timezone";
	mParams["fields"] += ",tv";
	mParams["fields"] += ",uid";
	mParams["fields"] += ",wall_count";
	mParams["fields"] += ",work_history";
}

void users_getInfo::handleResponse(istream & inputstream) const
{
	try
	{
		auto_ptr< users_getInfo_response > resp = users_getInfo_response_(inputstream, xml_schema::flags::dont_validate);
		mSuccessCallback.call(resp);
	}
	catch(...)
	{
		//mErrorCallback.call(...);
	}
}