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

#ifndef __OpcUaDiscovery_DiscoveryModelConfig_h__
#define __OpcUaDiscovery_DiscoveryModelConfig_h__

#include "OpcUaStackCore/Base/ConfigXmlManager.h"
#include "OpcUaStackCore/StandardDataTypes/ApplicationType.h"

using namespace OpcUaStackCore;

namespace OpcUaDiscovery
{

	class DiscoveryServerConfig
	{
	  public:
		DiscoveryServerConfig(void);
		~DiscoveryServerConfig(void);

		void configFileName(const std::string& configFileName);
		void elementPrefix(const std::string& elementPrefix);

		bool decode(Config& config);

		uint32_t offlineTimeout(void);
		uint32_t deleteTimeout(void);

	  private:
		std::string configFileName_;
		std::string elementPrefix_;

		uint32_t offlineTimeout_;
		uint32_t deleteTimeout_;
	};


	class DiscoveryEndpointConfig
	{
	  public:
		typedef boost::shared_ptr<DiscoveryEndpointConfig> SPtr;
		typedef std::vector<DiscoveryEndpointConfig::SPtr> Vec;
		typedef std::vector<std::string> DiscoveryUrls;

		DiscoveryEndpointConfig(void);
		~DiscoveryEndpointConfig(void);

		void configFileName(const std::string& configFileName);
		void elementPrefix(const std::string& elementPrefix);

		bool decode(Config& config);

		std::string serverUri(void);
		std::string productUri(void);
		ApplicationType applicationType(void);
		std::string serverName(void);
		std::string gatewayServerUri(void);
		DiscoveryUrls& discoveryUrls(void);
		std::string semaphoreFilePath(void);

	  private:
		std::string configFileName_;
		std::string elementPrefix_;

		std::string serverUri_;
		std::string productUri_;
		ApplicationType applicationType_;
		std::string serverName_;
		std::string gatewayServerUri_;
		DiscoveryUrls discoveryUrls_;
		std::string semaphoreFilePath_;
	};

	class DiscoveryModelConfig
	{
	  public:
		DiscoveryModelConfig(void);
		~DiscoveryModelConfig(void);

		void configFileName(const std::string& configFileName);
		void elementPrefix(const std::string& elementPrefix);

		bool decode(Config& config);

		DiscoveryServerConfig& discoveryServerConfig(void);
		DiscoveryEndpointConfig::Vec& discoveryEndpointConfigVec(void);

	  private:
		std::string configFileName_;
		std::string elementPrefix_;

		DiscoveryServerConfig discoveryServerConfig_;
		DiscoveryEndpointConfig::Vec discoveryEndpointConfigVec_;
	};

}

#endif
