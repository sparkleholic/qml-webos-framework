#ifndef SERVICEREQUEST_H
#define SERVICEREQUEST_H
  
#include <glib.h>
#include <string>
#include <luna-service2/lunaservice.h>
#include <pbnjson.hpp>
#include <PmLog.h>
  
class ServiceRequest
{
public:
    ServiceRequest(std::string appId);
    virtual ~ServiceRequest();
    LSHandle* getHandle() const { return m_serviceHandle; }
    void registerNativeApp();
  
protected:
    LSHandle* acquireHandle();
    void clearHandle();
  
private:
    GMainLoop* m_mainLoop;
    LSHandle* m_serviceHandle;
    std::string m_appId;
};
#endif
