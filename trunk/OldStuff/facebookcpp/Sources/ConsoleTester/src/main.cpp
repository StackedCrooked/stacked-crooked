#include "FacebookCpp/FacebookConnection.h"
#include "FacebookCpp/FacebookUtil.h"
#include "FacebookCpp/Method/AreFriends.h"
#include "FacebookCpp/Method/CreateAuthToken.h"
#include "FacebookCpp/Method/GetFriends.h"
#include "FacebookCpp/Method/GetSession.h"
#include "FacebookCpp/Method/users_getInfo.h"
#include "Poco/Bugcheck.h"
#include "Poco/String.h"
#include "Poco/Timer.h"
#include "XSD/facebook.hxx"
#include <iostream>



using namespace FacebookCpp;
using namespace FacebookCpp::XSD;
using namespace Poco;
using namespace std;


static const string cAppKey = "959af3e7f629a891ca0b6342f4bfe1a2";
static const string cAppSecret = "c4a35cfe3b77e2ae538623aed8b70365";


class ConsoleTester
{

public:
	ConsoleTester(const string & inAppKey, const string & inSecret, bool inMultiThreaded):
			mFacebookConnection(inAppKey, inSecret),
			mWaitTimer(0, 100),
			mIsTicking(false),
			mMultiThreaded(inMultiThreaded)
	{
	}


	void start()
	{
		cout << "Call CreateAuthToken";
		CreateAuthToken createToken(
			FbCallback<ConsoleTester, CreateAuthToken::ResponseType>(this, &ConsoleTester::createTokenSucceeded),
			FbCallback<ConsoleTester, ErrorResponse>(this, &ConsoleTester::createTokenFailed));
		mFacebookConnection.callMethod(createToken, mMultiThreaded);
		startTicking();
	}


	void createTokenSucceeded(const CreateAuthToken::ResponseType & inResponse)
	{
		stopTicking();
		mAuthentication = inResponse.token();
		cout << endl << "Auth: " << mAuthentication << endl;

		cout << "Login";
		mFacebookConnection.login(mAuthentication);
		
		cout << endl << "Browser is launched. Please log in and then, in this console window, press any key + enter to continue" << endl;
		char c; // block program flow by requesting console input
		cin >> c;

		cout << "Get session";
		mFacebookConnection.callMethod
		(
			GetSession
			(
				inResponse.token(),
				FbCallback<ConsoleTester, GetSession::ResponseType>(this, &ConsoleTester::getSessionSucceeded),
				FbCallback<ConsoleTester, ErrorResponse>(this, &ConsoleTester::getSessionFailed)
			),
			mMultiThreaded
		);
			
		startTicking();
	}


	void createTokenFailed(const ErrorResponse & inErrorResponse)
	{
		cout << inErrorResponse.errorMessage();
		PostQuitMessage(0);
	}


	void getSessionSucceeded(const Session & inResponse)
	{
		stopTicking();
		mSession = inResponse;
		cout << endl << "Session:" << endl;
		cout << "\tsessionKey: " << inResponse.sessionKey() << endl;
		cout << "\tuid: " << inResponse.uid() << endl;
		cout << "\texpires: " << inResponse.expires() << endl;
		cout << "\tsecret: " << inResponse.secret() << endl;


		cout << endl << "Getting Friends";
		mFacebookConnection.callMethod
		(
			GetFriends
			(
				inResponse,
				FbCallback<ConsoleTester, vector<string> >(this, &ConsoleTester::getFriendsSucceeded),
				FbCallback<ConsoleTester, ErrorResponse>(this, &ConsoleTester::getFriendsFailed)
			),
			mMultiThreaded
		);
		startTicking();
	}


	void getSessionFailed(const ErrorResponse & inErrorResponse)
	{
		stopTicking();
		cout << endl << inErrorResponse.errorMessage();
		PostQuitMessage(0);
	}


	void getFriendsSucceeded(const vector<string> & inFriendsList)
	{
		stopTicking();
		mFriends = inFriendsList;
		cout << endl << "Friends:" << endl;
		for(size_t idx = 0; idx != inFriendsList.size(); ++idx)
		{
			cout << "\t" << inFriendsList[idx] << endl;
		}
		

		cout << endl << "Get user info";

		std::vector<std::string> uids = inFriendsList;
		uids.push_back(mSession.uid());
		mFacebookConnection.callMethod
		(
			users_getInfo
			(
				mSession,
				uids,
				FbCallback<ConsoleTester, users_getInfo::ResponseType >(this, &ConsoleTester::users_getInfo_Succeeded),
				FbCallback<ConsoleTester, ErrorResponse>(this, &ConsoleTester::users_getInfo_Failed)
			),
			mMultiThreaded
		);

		startTicking();
	}


	void getFriendsFailed(const ErrorResponse & inErrorResponse)
	{
		stopTicking();
		cout << inErrorResponse.errorMessage();
		PostQuitMessage(0);
	}


	void users_getInfo_Succeeded(const users_getInfo::ResponseType & inUsersInfo)
	{
		stopTicking();
		users_getInfo_response * info = inUsersInfo.get();
		users_getInfo_response::user_sequence list = info->user();
		users_getInfo_response::user_sequence::iterator it = list.begin(), end = list.end();

		for(; it != end; ++it)
		{
			cout << endl;
			cout << (it->first_name().present() ? it->first_name().get() : "");
			cout << " " << (it->last_name().present() ? it->last_name().get() : "");

			if (it->hometown_location().present())
			{
				XSD::location loc = it->hometown_location().get();
				if (loc.city().present() && !loc.city().get().empty())
				{
					cout << ", " << loc.city().get();
				}
				if (loc.state().present() && !loc.state().get().empty())
				{
					cout << " - " << loc.state().get();
				}
				if (loc.country().present() && !loc.country().get().empty())
				{
					cout << " - " << loc.country().get();
				}
			}
		}
	}


	void users_getInfo_Failed(const ErrorResponse & inErrorResponse)
	{
		stopTicking();
		cout << endl << inErrorResponse.errorMessage();
		PostQuitMessage(0);
	}

	
	// Print dots during idle time.
	// If mMultiThreaded is true dots will also appear while waiting for server response.
	void startTicking()
	{
		if(!mIsTicking)
		{
			mIsTicking = true;
			mWaitTimer.setPeriodicInterval(1000);
			mWaitTimer.start(TimerCallback<ConsoleTester>(*this, &ConsoleTester::tickEvent));
		}
	}


	void stopTicking()
	{
		mWaitTimer.stop();
		mIsTicking = false;
	}


	void tickEvent(Timer & inTimer)
	{
		cout << ".";
	}


private:
	FacebookConnection mFacebookConnection;
	Timer mWaitTimer;
	bool mIsTicking;
	string mAuthentication;
	bool mMultiThreaded;
	vector<string> mFriends;
	Session mSession;
};


int main()
{	
	ConsoleTester tester(cAppKey, cAppSecret, true);
	tester.start();

	// App is multithreaded so message loop is needed
	MSG Msg;
	while( GetMessage(&Msg, NULL, 0, 0) > 0 )
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	cout << "End of program.";

	return 0;
}