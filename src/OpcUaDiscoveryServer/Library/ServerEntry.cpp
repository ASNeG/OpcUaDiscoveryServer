/*
   Copyright 2019 Kai Huebl (kai@huebl-sgh.de)

   Lizenziert gemäß Apache Licence Version 2.0 (die „Lizenz“); Nutzung dieser
   Datei nur in Übereinstimmung mit der Lizenz erlaubt.
   Eine Kopie der Lizenz erhalten Sie auf http://www.apache.org/licenses/LICENSE-2.0.

   Sofern nicht gemäß geltendem Recht vorgeschrieben oder schriftlich vereinbart,
   erfolgt die Bereitstellung der im Rahmen der Lizenz verbreiteten Software OHNE
   GEWÄHR ODER VORBEHALTE – ganz gleich, ob ausdrücklich oder stillschweigend.

   Informationen über die jeweiligen Bedingungen für Genehmigungen und Einschränkungen
   im Rahmen der Lizenz finden Sie in der Lizenz.

   Autor: Kai Huebl (kai@huebl-sgh.de)
 */

#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaDiscoveryServer/Library/ServerEntry.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaDiscoveryServer
{

	ServerEntry::ServerEntry(void)
	: lastUpdate_()
	, registeredServer_()
	, configEntry_(false)
	{
	}

	ServerEntry::~ServerEntry(void)
	{
	}

	void
	ServerEntry::configEntry(bool configEntry)
	{
		configEntry_ = configEntry;
	}

	bool
	ServerEntry::configEntry(void)
	{
		return configEntry_;
	}

	void
	ServerEntry::lastUpdate(boost::posix_time::ptime lastUpdate)
	{
		lastUpdate_ = lastUpdate;
	}

	boost::posix_time::ptime
	ServerEntry::lastUpdate(void)
	{
		return lastUpdate_;
	}

	RegisteredServer&
	ServerEntry::registeredServer(void)
	{
		return registeredServer_;
	}

}
