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

#include <boost/algorithm/string.hpp>
#include "OpcUaStackCore/Base/Log.h"
#include "OpcUaStackCore/Base/ObjectPool.h"
#include "OpcUaDiscoveryServer/Library/DiscoveryModelConfig.h"

using namespace OpcUaStackCore;

namespace OpcUaDiscovery
{

	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// DiscoveryConfig
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	DiscoveryServerConfig::DiscoveryServerConfig(void)
	: configFileName_("")
	, elementPrefix_("")
	, offlineTimeout_(600000)
	, deleteTimeout_(900000)
	{
	}

	DiscoveryServerConfig::~DiscoveryServerConfig(void)
	{
	}

	void
	DiscoveryServerConfig::configFileName(const std::string& configFileName)
	{
		configFileName_ = configFileName;
	}

	void
	DiscoveryServerConfig::elementPrefix(const std::string& elementPrefix)
	{
		elementPrefix_ = elementPrefix;
	}

	bool
	DiscoveryServerConfig::decode(Config& config)
	{
		bool success;

		// element OfflineTimeout
		success = config.getConfigParameter("OfflineTimeout", offlineTimeout_);
		if (!success) {
			Log(Error, "element missing in config file")
				.parameter("Element", elementPrefix_ + ".OfflineTimeout")
				.parameter("ConfigFileName", configFileName_);
			return false;
		}

		// element DeleteTimeout
		success = config.getConfigParameter("DeleteTimeout", deleteTimeout_);
		if (!success) {
			Log(Error, "element missing in config file")
				.parameter("Element", elementPrefix_ + ".DeleteTimeout")
				.parameter("ConfigFileName", configFileName_);
			return false;
		}

		return true;
	}

	uint32_t
	DiscoveryServerConfig::offlineTimeout(void)
	{
		return offlineTimeout_;
	}

	uint32_t
	DiscoveryServerConfig::deleteTimeout(void)
	{
		return deleteTimeout_;
	}


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// DiscoveryEndpointConfig
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	DiscoveryEndpointConfig::DiscoveryEndpointConfig(void)
	: configFileName_("")
	, elementPrefix_("")
	, serverUri_("")
	, productUri_("")
	, applicationType_(ApplicationType::EnumServer)
	, serverName_("")
	, gatewayServerUri_("")
	, discoveryUrls_()
	, semaphoreFilePath_("")
	{
	}

	DiscoveryEndpointConfig::~DiscoveryEndpointConfig(void)
	{
	}

	void
	DiscoveryEndpointConfig::configFileName(const std::string& configFileName)
	{
		configFileName_ = configFileName;
	}

	void
	DiscoveryEndpointConfig::elementPrefix(const std::string& elementPrefix)
	{
		elementPrefix_ = elementPrefix;
	}

	bool
	DiscoveryEndpointConfig::decode(Config& config)
	{
		bool success;

		// element ServerUri
		success = config.getConfigParameter("ServerUri", serverUri_);
		if (!success) {
			Log(Error, "element missing in config file")
				.parameter("Element", elementPrefix_ + ".ServerUri")
				.parameter("ConfigFileName", configFileName_);
			return false;
		}

		// element ProductUri
		success = config.getConfigParameter("ProductUri", productUri_);
		if (!success) {
			Log(Error, "element missing in config file")
				.parameter("Element", elementPrefix_ + ".ProductUri")
				.parameter("ConfigFileName", configFileName_);
			return false;
		}

		// element ApplicationType
		std::string applicationTypeStr;
		success = config.getConfigParameter("ApplicationType", applicationTypeStr);
		if (!success) {
			Log(Error, "element missing in config file")
				.parameter("Element", elementPrefix_ + ".ApplicationType")
				.parameter("ConfigFileName", configFileName_);
			return false;
		}
		if (applicationTypeStr == "Client") {
			applicationType_ = ApplicationType::EnumClient;
		}
		else if (applicationTypeStr == "Server") {
			applicationType_ = ApplicationType::EnumServer;
		}
		else if (applicationTypeStr == "ClientServer") {
			applicationType_ = ApplicationType::EnumClientAndServer;
		}
		else {
			Log(Error, "element invalid in config file")
				.parameter("Element", elementPrefix_ + ".ApplicationType")
				.parameter("Value", applicationTypeStr)
				.parameter("ConfigFileName", configFileName_);
			return false;
		}

		// element ServerName
		success = config.getConfigParameter("ServerName", serverName_);
		if (!success) {
			Log(Error, "element missing in config file")
				.parameter("Element", elementPrefix_ + ".ServerName")
				.parameter("ConfigFileName", configFileName_);
			return false;
		}

		// element GatewayServerUri
		success = config.getConfigParameter("GatewayServerUri", gatewayServerUri_);
		if (!success) {
			Log(Error, "element missing in config file")
				.parameter("Element", elementPrefix_ + ".ServerGatewayUri")
				.parameter("ConfigFileName", configFileName_);
			return false;
		}

		// element DiscoveryUrl
		config.getValues("DiscoveryUrl", discoveryUrls_);
		if (discoveryUrls_.size() == 0) {
			Log(Error, "element missing in config file")
				.parameter("Element", elementPrefix_ + ".DiscoveryUrl")
				.parameter("ConfigFileName", configFileName_);
			return false;
		}

		// element SemaphoreFilePath
		success = config.getConfigParameter("SemaphoreFilePath", semaphoreFilePath_);
		if (!success) {
			Log(Error, "element missing in config file")
				.parameter("Element", elementPrefix_ + ".SemaphoreFilePath")
				.parameter("ConfigFileName", configFileName_);
			return false;
		}

		return true;
	}

	std::string
	DiscoveryEndpointConfig::serverUri(void)
	{
		return serverUri_;
	}

	std::string
	DiscoveryEndpointConfig::productUri(void)
	{
		return productUri_;
	}

	ApplicationType
	DiscoveryEndpointConfig::applicationType(void)
	{
		return applicationType_;
	}

	std::string
	DiscoveryEndpointConfig::serverName(void)
	{
		return serverName_;
	}

	std::string
	DiscoveryEndpointConfig::gatewayServerUri(void)
	{
		return gatewayServerUri_;
	}

	DiscoveryEndpointConfig::DiscoveryUrls&
	DiscoveryEndpointConfig::discoveryUrls(void)
	{
		return discoveryUrls_;
	}

	std::string
	DiscoveryEndpointConfig::semaphoreFilePath(void)
	{
		return semaphoreFilePath_;
	}


	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	//
	// DiscoveryModelConfig
	//
	// ------------------------------------------------------------------------
	// ------------------------------------------------------------------------
	DiscoveryModelConfig::DiscoveryModelConfig(void)
	: configFileName_("")
	, elementPrefix_("")
	{
	}

	DiscoveryModelConfig::~DiscoveryModelConfig(void)
	{
	}

	void
	DiscoveryModelConfig::configFileName(const std::string& configFileName)
	{
		configFileName_ = configFileName;
	}

	void
	DiscoveryModelConfig::elementPrefix(const std::string& elementPrefix)
	{
		elementPrefix_ = elementPrefix;
	}

	bool
	DiscoveryModelConfig::decode(Config& config)
	{
		bool success;

		// element DiscoveryServer
		boost::optional<Config> dicoveryServerConfig = config.getChild("DiscoveryServer");
		if (!dicoveryServerConfig) {
			Log(Error, "element missing in config file")
				.parameter("Element", elementPrefix_ + ".DiscoveryServer")
				.parameter("ConfigFileName", configFileName_);
			return false;
		}

		discoveryServerConfig_.configFileName(configFileName_);
		discoveryServerConfig_.elementPrefix(elementPrefix_ + ".DiscoveryServer");
		if (!discoveryServerConfig_.decode(*dicoveryServerConfig)) {
			return false;
		}

		// element DiscoveryEndpoints.DiscoveryEndpoint
		std::vector<Config> childs;
		config.getChilds("DiscoveryEndpoints.DiscoveryEndpoint", childs);
		std::vector<Config>::iterator it;
		for (it = childs.begin(); it != childs.end(); it++) {
			DiscoveryEndpointConfig::SPtr discoveryEndpointConfig = constructSPtr<DiscoveryEndpointConfig>();
			discoveryEndpointConfig->configFileName(configFileName_);
			discoveryEndpointConfig->elementPrefix(elementPrefix_ + ".DiscoveryEndpoints.DiscoveryEndpoint");
			if (!discoveryEndpointConfig->decode(*it)) {
				return false;
			}
			discoveryEndpointConfigVec_.push_back(discoveryEndpointConfig);
		}

		return true;
	}

	DiscoveryServerConfig&
	DiscoveryModelConfig::discoveryServerConfig(void)
	{
		return discoveryServerConfig_;
	}

	DiscoveryEndpointConfig::Vec&
	DiscoveryModelConfig::discoveryEndpointConfigVec(void)
	{
		return discoveryEndpointConfigVec_;
	}

}
