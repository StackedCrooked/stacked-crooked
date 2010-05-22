//
// SessionImpl.cpp
//
// $Id: //poco/1.3/Data/ODBC/src/SessionImpl.cpp#5 $
//
// Library: Data/ODBC
// Package: ODBC
// Module:  SessionImpl
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Data/ODBC/SessionImpl.h"
#include "Poco/Data/ODBC/Utility.h"
#include "Poco/Data/ODBC/ODBCStatementImpl.h"
#include "Poco/Data/ODBC/Error.h"
#include "Poco/Data/ODBC/ODBCException.h"
#include <sqlext.h>


namespace Poco {
namespace Data {
namespace ODBC {


SessionImpl::SessionImpl(const std::string& connect,
	Poco::Any maxFieldSize, 
	bool enforceCapability,
	bool autoBind,
	bool autoExtract):
	_connect(connect),
	_maxFieldSize(maxFieldSize),
	_enforceCapability(enforceCapability),
	_autoBind(autoBind),
	_autoExtract(autoExtract)
{
	open();
}


SessionImpl::~SessionImpl()
{
	close();
}


Poco::Data::StatementImpl* SessionImpl::createStatementImpl()
{
	return new ODBCStatementImpl(*this);
}


void SessionImpl::begin()
{
}


void SessionImpl::open()
{
	POCO_SQLCHAR connectOutput[512] = {0};
	SQLSMALLINT result;

	if (Utility::isError(SQLDriverConnect(_db
		, NULL
		,(POCO_SQLCHAR*) _connect.c_str()
		,(SQLSMALLINT) SQL_NTS
		, connectOutput
		, sizeof(connectOutput)
		, &result
		, SQL_DRIVER_NOPROMPT)))
	{
		ConnectionException exc(_db);
		close();
		throw exc;
	}

	addFeature("enforceCapability", 
		&SessionImpl::setEnforceCapability, 
		&SessionImpl::getEnforceCapability);

	addFeature("autoCommit", 
		&SessionImpl::autoCommit, 
		&SessionImpl::isAutoCommit);

	addFeature("autoBind", 
		&SessionImpl::autoBind, 
		&SessionImpl::isAutoBind);

	addFeature("autoExtract", 
		&SessionImpl::autoExtract, 
		&SessionImpl::isAutoExtract);

	addProperty("maxFieldSize",
		&SessionImpl::setMaxFieldSize,
		&SessionImpl::getMaxFieldSize);

	if (_enforceCapability && !isCapable())
	{
		close();
		throw ODBCException("Connection closed "
			"(capability enforcement required but not all required functions supported).");
	}

	SQLSetConnectAttr(_db, SQL_ATTR_QUIET_MODE, 0, 0);

	if (!canTransact()) autoCommit("", true);
}


bool SessionImpl::canTransact()
{
	SQLUSMALLINT ret;
	checkError(SQLGetInfo(_db, SQL_TXN_CAPABLE, &ret, 0, 0), 
		"Failed to obtain transaction capability info.");

	return (SQL_TC_NONE != ret);
}


void SessionImpl::autoCommit(const std::string&, bool val)
{
	checkError(SQLSetConnectAttr(_db, 
		SQL_ATTR_AUTOCOMMIT, 
		val ? (SQLPOINTER) SQL_AUTOCOMMIT_ON : 
			(SQLPOINTER) SQL_AUTOCOMMIT_OFF, 
		0), "Failed to set automatic commit.");
}


bool SessionImpl::isAutoCommit(const std::string&)
{
	Poco::UInt32 value = 0;

	checkError(SQLGetConnectAttr(_db,
		SQL_ATTR_AUTOCOMMIT,
		&value,
		0,
		0));

	return (0 != value);
}


bool SessionImpl::isConnected()
{
	Poco::UInt32 value = 0;

	if (Utility::isError(SQLGetConnectAttr(_db,
		SQL_ATTR_CONNECTION_DEAD,
		&value,
		0,
		0)))
		return false;

	return (0 == value);
}


bool SessionImpl::isTransaction()
{
	Poco::UInt32 value = 0;

	checkError(SQLGetConnectAttr(_db,
		SQL_ATTR_AUTOCOMMIT,
		&value,
		0,
		0));

	return (0 == value);
}


void SessionImpl::close()
{
	if (!isConnected()) return;

	commit();
	checkError(SQLDisconnect(_db));
}


bool SessionImpl::isCapable()
{
	SQLUSMALLINT exists[FUNCTIONS] = {0};

	if (Utility::isError(SQLGetFunctions(_db, SQL_API_ODBC3_ALL_FUNCTIONS, exists)))
	{
		throw ConnectionException(_db,
			"SQLGetFunctions(SQL_API_ODBC3_ALL_FUNCTIONS)");
	}

	return SQL_FUNC_EXISTS(exists, SQL_API_SQLBINDPARAMETER)  &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLBINDCOL)        &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLGETDATA)        &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLPUTDATA)        &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLPARAMDATA)      &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLDESCRIBECOL)    &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLDESCRIBEPARAM)  &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLGETINFO)        &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLGETDIAGREC)     &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLGETDIAGFIELD)   &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLPREPARE)        &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLEXECUTE)        &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLEXECDIRECT)     &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLFETCH)          &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLNUMRESULTCOLS)  &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLALLOCHANDLE)    &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLFREEHANDLE)     &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLCLOSECURSOR)    &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLSETCONNECTATTR) &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLSETSTMTATTR)    &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLENDTRAN)        &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLNATIVESQL)      &&
		SQL_FUNC_EXISTS(exists, SQL_API_SQLCOLATTRIBUTE);
}


int SessionImpl::maxStatementLength()
{
	SQLUINTEGER info;
	SQLRETURN rc = 0;
	if (Utility::isError(rc = SQLGetInfo(_db,
		SQL_MAXIMUM_STATEMENT_LENGTH,
		(SQLPOINTER) &info,
		0,
		0)))
	{
		throw ConnectionException(_db, 
			"SQLGetInfo(SQL_MAXIMUM_STATEMENT_LENGTH)");
	}

	return info;
}


void SessionImpl::setEnforceCapability(const std::string&, bool val)
{
	_enforceCapability = val;
}


bool SessionImpl::getEnforceCapability(const std::string&)
{
	return _enforceCapability;
}


void SessionImpl::autoBind(const std::string&, bool val)
{
	_autoBind = val;
}


bool SessionImpl::isAutoBind(const std::string& name)
{
	return _autoBind;
}


void SessionImpl::autoExtract(const std::string&, bool val)
{
	_autoExtract = val;
}


bool SessionImpl::isAutoExtract(const std::string& name)
{
	return _autoExtract;
}


} } } // namespace Poco::Data::ODBC
