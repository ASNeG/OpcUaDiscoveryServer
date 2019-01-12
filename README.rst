OpcUaDiscoveryServer
========================================================

Installation
--------------------------------------------------------

In order to install OpcUaDiscoveryServer in your machine type the following command:

**On Linux** 

::
    $ sh build.sh -t local
	 
	
**On Windows**

::
    $ build.bat local


Docker
--------------------------------------------------------

You can use Docker to run OpcUaDiscoveryServer:

::
    $ docker build -t OpcUaDiscoveryServer:latest .
    $ docker run -d -p 4840:4840 OpcUaDiscoveryServer:latest
