#include <unistd.h>
#include "napi/helper/Js_Cocos2dxHelper.h"
#include "napi/helper/NapiHelper.h"
#include "CCSDKApplication.h"
#include "CCSDKLogOhos.h"
#include <string>

cocos2d::MsgCallback cocos2d::SDKApplication::msgCb;
std::string cocos2d::SDKApplication::loginSDKResult="";
std::string cocos2d::SDKApplication::paySDKResult="";
cocos2d::SDKApplication *cocos2d::SDKApplication::sm_pSharedApplication = nullptr;
cocos2d::SDKApplication::SDKApplication() {
    sm_pSharedApplication = this;
}

cocos2d::SDKApplication::~SDKApplication() {
    sm_pSharedApplication = nullptr;
}

cocos2d::SDKApplication *cocos2d::SDKApplication::getInstance() {
    return sm_pSharedApplication;
}
std::string cocos2d::SDKApplication::getLginSDKResult() { return loginSDKResult; }

std::string cocos2d::SDKApplication::loginInit()
{
    JSFunction::getFunction("LoginSDK.loginInit").invoke<std::string>();
    OHOS_LOGD( "==========loginInit executeLoginSDKCallback1 is [%{public}s] =========", loginSDKResult.c_str());
    return loginSDKResult;
}

std::string cocos2d::SDKApplication::on()
{
    JSFunction::getFunction("LoginSDK.on").invoke<std::string>();
    OHOS_LOGD( "==========on executeLoginSDKCallback1 is [%{public}s] =========", loginSDKResult.c_str());
    return loginSDKResult;
}

std::string cocos2d::SDKApplication::unionLogin()
{
    JSFunction::getFunction("LoginSDK.unionLogin").invoke<std::string>();
    OHOS_LOGD( "==========unionLogin executeLoginSDKCallback1 is [%{public}s] =========", loginSDKResult.c_str());
    return loginSDKResult;
}

std::string cocos2d::SDKApplication::authorization()
{
    JSFunction::getFunction("LoginSDK.authorization").invoke<std::string>();
    OHOS_LOGD( "==========authorization executeLoginSDKCallback1 is [%{public}s] =========", loginSDKResult.c_str());
    return loginSDKResult;
}

std::string cocos2d::SDKApplication::playerInfoVerification()
{
    JSFunction::getFunction("LoginSDK.playerInfoVerification").invoke<std::string>();
    OHOS_LOGD( "==========playerInfoVerification executeLoginSDKCallback1 is [%{public}s] =========", loginSDKResult.c_str());
    return loginSDKResult;
}

std::string cocos2d::SDKApplication::bindPlayer()
{
    JSFunction::getFunction("LoginSDK.bindPlayer").invoke<std::string>();
    OHOS_LOGD( "==========bindPlayer executeLoginSDKCallback1 is [%{public}s] =========", loginSDKResult.c_str());
    return loginSDKResult;
}

std::string cocos2d::SDKApplication::verifyLocalPlayer()
{
    JSFunction::getFunction("LoginSDK.verifyLocalPlayer").invoke<std::string>();
    OHOS_LOGD( "==========verifyLocalPlayer executeLoginSDKCallback1 is [%{public}s] =========", loginSDKResult.c_str());
    return loginSDKResult;
}

std::string cocos2d::SDKApplication::savePlayerRole()
{
    JSFunction::getFunction("LoginSDK.savePlayerRole").invoke<std::string>();
    OHOS_LOGD( "==========savePlayerRole executeLoginSDKCallback1 is [%{public}s] =========", loginSDKResult.c_str());
    return loginSDKResult;
}

std::string cocos2d::SDKApplication::authorization2()
{
    JSFunction::getFunction("LoginSDK.authorization2").invoke<std::string>();
    OHOS_LOGD( "==========authorization2 executeLoginSDKCallback1 is [%{public}s] =========", loginSDKResult.c_str());
    return loginSDKResult;
}

std::string cocos2d::SDKApplication::verifyLocalPlayer2()
{
    JSFunction::getFunction("LoginSDK.verifyLocalPlayer2").invoke<std::string>();
    OHOS_LOGD( "==========verifyLocalPlayer2 executeLoginSDKCallback1 is [%{public}s] =========", loginSDKResult.c_str());
    return loginSDKResult;
}

std::string cocos2d::SDKApplication::savePlayerRole2()
{
    JSFunction::getFunction("LoginSDK.savePlayerRole2").invoke<std::string>();
    OHOS_LOGD( "==========savePlayerRole2 executeLoginSDKCallback1 is [%{public}s] =========", loginSDKResult.c_str());
    return loginSDKResult;
}

std::string cocos2d::SDKApplication::executeLoginSDKCallback(const char * result)
{
    loginSDKResult = std::string(result);
    msgCb(result);
    OHOS_LOGD("==========pullSDK executeLoginSDKCallback2 is [%{public}s] =========",loginSDKResult.c_str());
    return loginSDKResult;
}


std::string cocos2d::SDKApplication::checkPayEnv()
{
    JSFunction::getFunction("PaySDK.checkPayEnv").invoke<std::string>();
    return "";
}
std::string cocos2d::SDKApplication::queryConsumProdts()
{
    JSFunction::getFunction("PaySDK.queryConsumProdts").invoke<std::string>();
    return "";
}
std::string cocos2d::SDKApplication::buy()
{
    JSFunction::getFunction("PaySDK.buy").invoke<std::string>();
    return "";
}

std::string cocos2d::SDKApplication::queryNoConsumableGoods()
{
    JSFunction::getFunction("PaySDK.queryNoConsumableGoods").invoke<std::string>();
    return "";
}
std::string cocos2d::SDKApplication::purchaseOfNonConsumableGoods()
{
    JSFunction::getFunction("PaySDK.purchaseOfNonConsumableGoods").invoke<std::string>();
    return "";
}
std::string cocos2d::SDKApplication::disposableGoodsOrderQuery()
{
    JSFunction::getFunction("PaySDK.disposableGoodsOrderQuery").invoke<std::string>();
    return "";
}
std::string cocos2d::SDKApplication::nonConsumableProductsRecords()
{
    JSFunction::getFunction("PaySDK.nonConsumableProductsRecords").invoke<std::string>();
    return "";
}
std::string cocos2d::SDKApplication::consumableProductsRecords()
{
    JSFunction::getFunction("PaySDK.consumableProductsRecords").invoke<std::string>();
    return "";
}
std::string cocos2d::SDKApplication::noConsumGoodsService() {
    JSFunction::getFunction("PaySDK.noConsumGoodsService").invoke<std::string>();
    return "";
}
std::string cocos2d::SDKApplication::queryAutorenewGoods() {
    JSFunction::getFunction("PaySDK.queryAutorenewGoods").invoke<std::string>();
    return "";
}
std::string cocos2d::SDKApplication::purchaseOfAutoRenewGoods() {
    JSFunction::getFunction("PaySDK.purchaseOfAutoRenewGoods").invoke<std::string>();
    return "";
}
std::string cocos2d::SDKApplication::autoRenewProductsRecords() {
    JSFunction::getFunction("PaySDK.autoRenewProductsRecords").invoke<std::string>();
    return "";
}
std::string cocos2d::SDKApplication::autoRenewGoodsService() {
    JSFunction::getFunction("PaySDK.autoRenewGoodsService").invoke<std::string>();
    return "";
}

std::string cocos2d::SDKApplication::executePaySDKCallback(const char * result)
{
    paySDKResult = std::string(result);
    OHOS_LOGD("==========pullSDK executePaySDKCallback2 is [%{public}s] =========",paySDKResult.c_str());
    return paySDKResult;
}
std::string cocos2d::SDKApplication::getPaySDKResult() { return paySDKResult; }
