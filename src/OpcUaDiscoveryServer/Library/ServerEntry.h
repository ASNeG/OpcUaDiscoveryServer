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

#ifndef __OpcUaDiscovery_ServerEntry_h__
#define __OpcUaDiscovery_ServerEntry_h__

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "OpcUaStackCore/StandardDataTypes/RegisteredServer.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaDiscoveryServer
{

	class ServerEntry
	{
	  public:
		typedef boost::shared_ptr<ServerEntry> SPtr;
		typedef std::map<std::string, ServerEntry::SPtr> Map;

		ServerEntry(void);
		~ServerEntry(void);

		void configEntry(bool configEntry);
		bool configEntry(void);
		void lastUpdate(boost::posix_time::ptime lastUpdate);
		boost::posix_time::ptime lastUpdate(void);
		RegisteredServer& registeredServer(void);

	  private:
		bool configEntry_;
		boost::posix_time::ptime lastUpdate_;
		RegisteredServer registeredServer_;
	};

}

#endif
