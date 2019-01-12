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

#include "OpcUaStackCore/Base/os.h"
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaDiscoveryServer/Library/Library.h"
#include "OpcUaStackServer/ServiceSetApplication/ApplicationService.h"
#include "OpcUaStackServer/ServiceSetApplication/NodeReferenceApplication.h"
#include <iostream>
#include "BuildConfig.h"

namespace OpcUaDiscoveryServer
{

	Library::Library(void)
	: ApplicationIf()
	, configXmlManager_()
	, discovery_()
	{
	}

	Library::~Library(void)
	{
	}

	bool
	Library::startup(void)
	{
		//
		// create own thread
		//
		ioThread_ = constructSPtr<IOThread>();
		if (!ioThread_->startup()) return false;

		//
        // read discovery model configuration file
		//
		Config::SPtr config;
        if (!configXmlManager_.registerConfiguration(applicationInfo()->configFileName(), config)) {
            Log(Error, "read configuration file error")
            	.parameter("ErrorMessage", configXmlManager_.errorMessage())
                .parameter("ConfigFileName", applicationInfo()->configFileName());
        	return false;
        }
        Log(Info, "read configuration file")
            .parameter("ConfigFileName", applicationInfo()->configFileName());

        // element DiscoveryModel
		boost::optional<Config> discoveryModelCfg = config->getChild("DiscoveryModel");
		if (!discoveryModelCfg) {
			Log(Error, "element missing in config file")
				.parameter("Element", "DiscoveryModel")
				.parameter("ConfigFileName", applicationInfo()->configFileName());
			return false;
		}

        // parse discovery model config
        discoveryModelConfig_.configFileName(applicationInfo()->configFileName());
        discoveryModelConfig_.elementPrefix("DiscoveryModel");
        if (!discoveryModelConfig_.decode(*discoveryModelCfg)) {
        	return false;
        }

        //
        // startup discovery service
        //
		discovery_.applicationServiceIf(&service());
		discovery_.ioThread(ioThread_);
		discovery_.discoveryModelConfig(discoveryModelConfig_);
		if (!discovery_.startup()) {
			return false;
		}

		return true;
	}

	bool
	Library::shutdown(void)
	{
		Log(Debug, "Library::shutdown");

		//
		// shutdown discovery service
		//
		if (!discovery_.shutdown()) {
			return false;
		}

		//
		// stop threads
		//
	    if (!ioThread_->shutdown()) {
	        return false;
	    }
	    ioThread_.reset();

		return true;
	}

	std::string
	Library::version(void)
	{
		std::stringstream version;

		version << LIBRARY_VERSION_MAJOR << "." << LIBRARY_VERSION_MINOR << "." << LIBRARY_VERSION_PATCH;
		return version.str();
	}

}

extern "C" DLLEXPORT void  init(ApplicationIf** applicationIf) {
    *applicationIf = new OpcUaDiscoveryServer::Library();
}

