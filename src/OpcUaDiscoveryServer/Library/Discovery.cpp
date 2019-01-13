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
#include "OpcUaStackServer/ServiceSetApplication/RegisterForwardGlobal.h"
#include "OpcUaDiscoveryServer/Library/Discovery.h"

using namespace OpcUaStackCore;
using namespace OpcUaStackServer;

namespace OpcUaDiscoveryServer
{

	Discovery::Discovery(void)
	: applicationServiceIf_(nullptr)
	, serverEntryMap_()
	, mutex_()
	, ioThread_()
	, slotTimerElement_()
	, discoveryModelConfig_(nullptr)
	{
	}

	Discovery::~Discovery(void)
	{
	}

	void
	Discovery::applicationServiceIf(ApplicationServiceIf* applicationServiceIf)
	{
		applicationServiceIf_ = applicationServiceIf;
	}

	void
	Discovery::ioThread(IOThread::SPtr& ioThread)
	{
		ioThread_ = ioThread;
	}

	void
	Discovery::discoveryModelConfig(DiscoveryModelConfig& discoveryModelConfig)
	{
		discoveryModelConfig_ = &discoveryModelConfig;
	}

    bool
    Discovery::startup(void)
    {
		RegisterForwardGlobal registerForwardGlobal;
		registerForwardGlobal.setRegisterServerCallback(boost::bind(&Discovery::registerServer, this, _1));
		registerForwardGlobal.setFindServersCallback(boost::bind(&Discovery::findServer, this, _1));
		if (!registerForwardGlobal.query(applicationServiceIf_)) {
	  		Log(Error, "register forward response error");
			return false;
		}

		registerServerConfig(discoveryModelConfig_->discoveryEndpointConfigVec());

	  	// start timer to check server entries
	  	slotTimerElement_ = constructSPtr<SlotTimerElement>();
	  	slotTimerElement_->callback().reset(boost::bind(&Discovery::monitorServerEntries, this));
	  	slotTimerElement_->expireTime(boost::posix_time::microsec_clock::local_time(), 5000);
	  	ioThread_->slotTimer()->start(slotTimerElement_);

    	return true;
    }

    bool
    Discovery::shutdown(void)
    {
    	// stop timer
    	if (slotTimerElement_.get() != nullptr) {
    		ioThread_->slotTimer()->stop(slotTimerElement_);
    		slotTimerElement_.reset();
    	}

    	return true;
    }

    void
    Discovery::registerServerConfig(DiscoveryEndpointConfig::Vec& discoveryEndpointConfigVec)
    {
    	DiscoveryEndpointConfig::Vec::iterator it;
    	for (it = discoveryEndpointConfigVec.begin(); it != discoveryEndpointConfigVec.end(); it++) {
    		DiscoveryEndpointConfig::SPtr dec = *it;

    		// create new server entry
    		ServerEntry::SPtr serverEntry = constructSPtr<ServerEntry>();
    		serverEntry->lastUpdate(boost::posix_time::microsec_clock::universal_time());
    		serverEntry->configEntry(true);

    		// server uri
    		serverEntry->registeredServer().serverUri() = dec->serverUri();

    		// product uri
    		serverEntry->registeredServer().productUri() = dec->productUri();

    		// server name
    		OpcUaLocalizedText::SPtr serverName = constructSPtr<OpcUaLocalizedText>();
    		serverName->set("en", dec->serverName());
    		OpcUaLocalizedTextArray::SPtr serverNames = constructSPtr<OpcUaLocalizedTextArray>();
    		serverNames->resize(1);
    		serverNames->push_back(serverName);
    		serverEntry->registeredServer().serverNames() = *serverNames;

    		// server type
    		serverEntry->registeredServer().serverType() = dec->applicationType();

    		// gateway server uri
    		serverEntry->registeredServer().gatewayServerUri() = dec->gatewayServerUri();

    		// discovery urls
    		OpcUaStringArray::SPtr discoveryUrls = constructSPtr<OpcUaStringArray>();
    		discoveryUrls->resize(dec->discoveryUrls().size());
    		DiscoveryEndpointConfig::DiscoveryUrls::iterator it;
    		for (it = dec->discoveryUrls().begin(); it != dec->discoveryUrls().end(); it++) {
    			OpcUaString::SPtr discoveryUrl = constructSPtr<OpcUaString>();
    			discoveryUrl->value(*it);
    			discoveryUrls->push_back(discoveryUrl);
    		}
    		serverEntry->registeredServer().discoveryUrls() = *discoveryUrls;

    		// semaphore file path
    		serverEntry->registeredServer().semaphoreFilePath() = dec->semaphoreFilePath();

    		// is online
    		serverEntry->registeredServer().isOnline() = true;

    		// insert new entry into server entry map
    		serverEntryMap_.insert(
    			std::make_pair(
    				serverEntry->registeredServer().serverUri().value(),
    				serverEntry
    			)
    		);
    	}
    }

    void
    Discovery::registerServer(ApplicationRegisterServerContext* applicationRegisterServerContext)
    {
    	boost::mutex::scoped_lock g(mutex_);

       	//
        // String serverUri - global unique server instance
    	// String productUri - global unique product identifier
    	// LocalizedText serverNames[] - descriptive names of the server
        // Enum serverType - type of application
    	// String gatewayServerUri - the uri of the gateway server associated
    	//							 with the discovery urls
    	// String discoveryUrls[] - a list of discovery endpoints for the server
    	// String semaphoreFilePath - the path of the semaphore file used to identify
    	//                            an automatically-launched server instance
    	// Boolean isOnline - true if the server is currently able to accept
    	//                    connections from clients


    	Log(Debug, "register server request")
    	    .parameter("ServerUri", applicationRegisterServerContext->server_.serverUri().value())
    	    .parameter("ProductUri", applicationRegisterServerContext->server_.productUri().value())
    	    //.parameter("ServerNames", applicationRegisterServerContext->server_.serverNames())
    	    .parameter("ServerType", applicationRegisterServerContext->server_.serverType().toString())
    	    .parameter("GatewayServerUri", applicationRegisterServerContext->server_.gatewayServerUri())
    	    //.parameter("DiscoveryUrls", applicationRegisterServerContext->server_.discoveryUrls())
    	    .parameter("SemaphoreFilePath", applicationRegisterServerContext->server_.semaphoreFilePath())
    	    .parameter("IsOnline", applicationRegisterServerContext->server_.isOnline());

    	// find existing server entry
    	ServerEntry::Map::iterator it;
    	it = serverEntryMap_.find(applicationRegisterServerContext->server_.serverUri().value());
    	if (it == serverEntryMap_.end()) {
    		// create new server entry
    		ServerEntry::SPtr serverEntry = constructSPtr<ServerEntry>();
    		serverEntry->lastUpdate(boost::posix_time::microsec_clock::universal_time());
    		applicationRegisterServerContext->server_.copyTo(serverEntry->registeredServer());

    		serverEntryMap_.insert(
    			std::make_pair(
    				applicationRegisterServerContext->server_.serverUri().toStdString(),
    				serverEntry
    			)
    		);
    	}
    	else {
    		// update server entry
    		ServerEntry::SPtr serverEntry = it->second;
    		serverEntry->lastUpdate(boost::posix_time::microsec_clock::universal_time());
    		applicationRegisterServerContext->server_.copyTo(serverEntry->registeredServer());
    	}

    	applicationRegisterServerContext->statusCode_ = Success;

    }

    void
    Discovery::findServer(ApplicationFindServerContext* applicationFindServerContext)
    {
    	boost::mutex::scoped_lock g(mutex_);

       	//
    	// registered server data structure:
    	//
        // String serverUri - global unique server instance
    	// String productUri - global unique product identifier
    	// LocalizedText serverNames[] - descriptive names of the server
        // Enum serverType - type of application
    	// String gatewayServerUri - the uri of the gateway server associated
    	//							 with the discovery urls
    	// String discoveryUrls[] - a list of discovery endpoints for the server
    	// String semaphoreFilePath - the path of the semaphore file used to identify
    	//                            an automatically-launched server instance
    	// Boolean isOnline - true if the server is currently able to accept
    	//                    connections from clients


    	//
    	// find servers request:
    	//
    	// String EndpointUrl - the network address that the client used to access
    	//                      the discovery endpoint
    	// LocaleId localIds[] - list of locales to use.
    	// String serverUris[] - list of servers to return. All known servers are
    	//                       returned if the list is empty
    	//

    	//
    	// find servers response
    	//
    	// ApplicationDescription servers[]
    	//   String applicationUri - global unique server instance
    	//   String productUri - global unique product identifier
    	//   LocalizedText applicationName - a localized descriptive name of
    	//                                   the application
    	//   Enum applicationType - type of application
    	//   String gatewayUri - the uri of the gateway server associated
    	//					     with the discovery urls
    	//   String discoveryProfileUri - a uri that identifies the discovery profile
    	//                                supported by the urls provided
    	//   String discoveryUrls[] - a list of urls for the discovery endpoints
    	//                            provided by the applications
    	//


    	Log(Debug, "find server request")
			.parameter("EndpointUrl", applicationFindServerContext->endpointUrl_)
			.parameter("LocaleIds", *applicationFindServerContext->localeIdArraySPtr_)
			.parameter("ServerUris", *applicationFindServerContext->serverUriArraySPtr_);

    	// select server entries
    	ApplicationDescription::Vec adVec;
    	ServerEntry::Map::iterator it;
    	for (it = serverEntryMap_.begin(); it != serverEntryMap_.end(); it++) {
    		ServerEntry::SPtr serverEntry = it->second;

    		// process filter
    		bool filterResult = processFilter(
    			serverEntry,
    			applicationFindServerContext->endpointUrl_,
    			applicationFindServerContext->localeIdArraySPtr_,
    			applicationFindServerContext->serverUriArraySPtr_
    		);
    		if (!filterResult) {
    			continue;
    		}

    		// create application description
    		ApplicationDescription::SPtr ad = constructSPtr<ApplicationDescription>();
    		ad->applicationUri() = serverEntry->registeredServer().serverUri();
    		ad->productUri() = serverEntry->registeredServer().productUri();

    		if (serverEntry->registeredServer().serverNames().size() > 0) {
    			OpcUaLocalizedText::SPtr serverName;
    			serverEntry->registeredServer().serverNames().get(0, serverName);
    			ad->applicationName() = *serverName;
    		}

    		ad->applicationType() = serverEntry->registeredServer().serverType();
    		ad->gatewayServerUri() = serverEntry->registeredServer().gatewayServerUri();

    		// discoveryProfileUri - todo

    		if (serverEntry->registeredServer().discoveryUrls().size() > 0) {
    			ad->discoveryUrls() = serverEntry->registeredServer().discoveryUrls();
    		}

    		adVec.push_back(ad);
    	}

    	applicationFindServerContext->servers_ = constructSPtr<ApplicationDescriptionArray>();
    	if (adVec.size() > 0) {
    	    applicationFindServerContext->servers_->resize(adVec.size());

    	    ApplicationDescription::Vec::iterator it;
    	    for (it = adVec.begin(); it != adVec.end(); it++) {
    	    	ApplicationDescription::SPtr ad = *it;
    	    	applicationFindServerContext->servers_->push_back(ad);
    	    }
    	}

    	applicationFindServerContext->statusCode_ = Success;
    }

    void
    Discovery::monitorServerEntries(void)
    {
		Log(Debug, "check server timeouts");

    	boost::mutex::scoped_lock g(mutex_);

    	boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();

    	ServerEntry::Map::iterator it;
    	for (it = serverEntryMap_.begin(); it != serverEntryMap_.end();) {
    		ServerEntry::SPtr serverEntry = it->second;

    		// ignore configuration entries
    		if (serverEntry->configEntry()) {
    			it++;
    			continue;
    		}

    		boost::posix_time::time_duration dt = now - serverEntry->lastUpdate();

    		// check offline timer
    		if (dt.total_milliseconds() > discoveryModelConfig_->discoveryServerConfig().offlineTimeout()) {
    			if (serverEntry->registeredServer().isOnline()) {
    				Log(Debug, "server is going offline")
    					.parameter("ServerUri", it->first);
    				serverEntry->registeredServer().isOnline() = false;
    			}
    		}

    		// check delete timer
    		if (dt.total_milliseconds() > discoveryModelConfig_->discoveryServerConfig().deleteTimeout()) {
    			Log(Debug, "server delete")
    				.parameter("ServerUri", it->first);
    			serverEntryMap_.erase(it++);
    		}
    		else {
    			it++;
    		}
    	}
    }

    bool
    Discovery::processFilter(
    	ServerEntry::SPtr& serverEntry,
    	OpcUaString& endpointUrl,
    	OpcUaStringArray::SPtr& localeIdArray,
    	OpcUaStringArray::SPtr& serverUriArray
    )
    {
    	if (!serverEntry->registeredServer().isOnline()) {
    		return false;
    	}

    	// FIXME: todo

    	return true;
    }

}
