#include "ServiceRequest.h"
  
static PmLogContext getPmLogContext()
{
    static PmLogContext s_context = 0;
    if (0 == s_context)
    {
        PmLogGetContext("NativeQtApp", &s_context);
    }
    return s_context;
}
  
static pbnjson::JValue convertStringToJson(const char *rawData)
{
    pbnjson::JInput input(rawData);
    pbnjson::JSchema schema = pbnjson::JSchemaFragment("{}");
    pbnjson::JDomParser parser;
    if (!parser.parse(input, schema))
    {
        return pbnjson::JValue();
    }
    return parser.getDom();
}
  
static std::string convertJsonToString(const pbnjson::JValue json)
{
    return pbnjson::JGenerator::serialize(json, pbnjson::JSchemaFragment("{}"));
}
  
ServiceRequest::ServiceRequest(std::string appId)
    : m_mainLoop(g_main_loop_new(nullptr, false))
    , m_serviceHandle(nullptr)
{
    m_appId = appId;
    m_serviceHandle = acquireHandle();
}
  
ServiceRequest::~ServiceRequest()
{
    clearHandle();
    if (m_mainLoop)
    {
        g_main_loop_quit(m_mainLoop); // optional!
        g_main_loop_unref(m_mainLoop);
        m_mainLoop = nullptr;
    }
}
  
LSHandle* ServiceRequest::acquireHandle()
{
    LSError lserror;
    LSErrorInit(&lserror);
  
    LSHandle* handle = nullptr;
    if (!LSRegister(m_appId.c_str(), &handle, &lserror))
    {
        LSErrorPrint(&lserror, stderr);
        return nullptr;       
    }
      
    if (!LSGmainAttach(handle, m_mainLoop, &lserror))
    {
        LSErrorPrint(&lserror, stderr);
        return nullptr;
    }
    return handle;
}
  
void ServiceRequest::clearHandle()
{
    LSError lserror;
    LSErrorInit(&lserror);
    if (m_serviceHandle)
    {
        LSUnregister(m_serviceHandle, &lserror);
        m_serviceHandle = nullptr;
    }
}
  
static bool registerNativeAppCallback(LSHandle* sh, LSMessage* msg, void* ctx)
{
    PmLogInfo(getPmLogContext(), "REGISTER_CALLBACK", 1, PMLOGJSON("payload", LSMessageGetPayload(msg)),  " ");
      
    pbnjson::JValue response = convertStringToJson(LSMessageGetPayload(msg));
    bool successCallback = response["returnValue"].asBool();
    if (successCallback)
    {
        std::string message = response["message"].asString();
        PmLogInfo(getPmLogContext(), "REGISTER_CALLBACK", 1, PMLOGKS("message", message.c_str()),  " ");
  
        if (message == "registered")
        {           
            // first launch..
        }
        else if (message == "relaunch")
        {
            if (response.hasKey("parameters"))
            {
                pbnjson::JValue launchParams = response["parameters"];
                PmLogInfo(getPmLogContext(), "REGISTER_CALLBACK", 1,
                            PMLOGJSON("parameters", convertJsonToString(launchParams).c_str()),  " ");
                  
                // process launch params..
            }
        }
    }
    else
    {
        PmLogError(getPmLogContext(), "REGISTER_CALLBACK", 0, "RegisterNativeApp Callback Error" );
        // error handling..
    }
    return true;
}
  
void ServiceRequest::registerNativeApp()
{
    PmLogInfo(getPmLogContext(), "APP_REGISTER", 0, "Register Native app");
    LSError lserror;
    LSErrorInit(&lserror);
    LSHandle* handle = getHandle();
    if (!handle) {
        PmLogError(getPmLogContext(), "APP_REGISTER", 0, "LSHandle is NULL" );
    }
  
    if (!LSCall(handle,
                "luna://com.webos.service.applicationmanager/registerNativeApp",
                "{}",
                registerNativeAppCallback,
                NULL,
                NULL,
                &lserror))
    {
        LSErrorPrint(&lserror, stderr);
    }
}
