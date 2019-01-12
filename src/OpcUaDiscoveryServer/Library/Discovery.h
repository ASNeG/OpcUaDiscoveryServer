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

#ifndef __OpcUaDiscovery_Discovery_h__
#define __OpcUaDiscovery_Discovery_h__

#include "OpcUaStackCore/Base/Config.h"
#include "OpcUaStackCore/Application/ApplicationRegisterServerContext.h"
#include "OpcUaStackCore/Application/ApplicationFindServerContext.h"
#include "OpcUaStackServer/Application/ApplicationIf.h"
#include "OpcUaDiscoveryServer/Library/ServerEntry.h"
#include "OpcUaDiscoveryServer/Library/DiscoveryModelConfig.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaDiscovery
{

	class Discovery
	{
	  public:

		Discovery(void);
		~Discovery(void);

		void applicationServiceIf(ApplicationServiceIf* applicationServiceIf);
		void ioThread(IOThread::SPtr& iocThread);
		void discoveryModelConfig(DiscoveryModelConfig& discoveryModelConfig);

	    bool startup(void);
	    bool shutdown(void);

	  private:
	    void registerServerConfig(DiscoveryEndpointConfig::Vec& discoveryEndpointConfigVec);
	    void registerServer(ApplicationRegisterServerContext* applicationRegisterServerContext);
	    void findServer(ApplicationFindServerContext* applicationFindServerContext);
	    void monitorServerEntries(void);
	    bool processFilter(
	    	ServerEntry::SPtr& serverEntry,
	    	OpcUaString& endpointUrl,
	    	OpcUaStringArray::SPtr& localeIdArray,
	    	OpcUaStringArray::SPtr& serverUriArray
	    );

	    ApplicationServiceIf* applicationServiceIf_;
	    IOThread::SPtr ioThread_;
	    SlotTimerElement::SPtr slotTimerElement_;
	    DiscoveryModelConfig* discoveryModelConfig_;

	    boost::mutex mutex_;
	    ServerEntry::Map serverEntryMap_;
	};

}

#endif
